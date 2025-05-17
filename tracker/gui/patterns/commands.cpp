#include "../../general.hpp"
#include "../../utils/string.hpp"
#include "../mapping.hpp"
#include "commands.hpp"

CommandsPattern::CommandsPattern()
    : commands_handler(
          commands,
          current_row,
          std::vector<ImGuiKey>(std::begin(commands_keys), std::end(commands_keys))
      ),
      values_handler(
          values,
          current_row,
          std::vector<ImGuiKey>(std::begin(values_keys), std::end(values_keys)),
          false
      ) {
    commands_handler.set_limit(MAX_COMMAND_COMMAND_SIZE);
    values_handler.set_limit(MAX_COMMAND_VALUE_SIZE);
}

void CommandsPattern::from_sequence(const uint8_t index) {
    sequence_index = index;
    const CommandsSequence *sequence = commands_sequences[sequence_index];
    uint16_t total_length = 0;

    commands.clear();
    values.clear();
    durations.clear();
    for (size_t i = 0; i < sequence->length; ++i) {
        const Command command = sequence->commands[i];
        switch (static_cast<Instruction>(command.instruction)) {
        case Instruction::PortamentoUp: {
            const CommandPortamentoUp portamento_up = reinterpret_cast<const CommandPortamentoUp &>(command);
            const std::string command_value = from_portamento(portamento_up.channel, portamento_up.value);
            add_command("U", command_value);
            break;
        }
        case Instruction::PortamentoDown: {
            const CommandPortamentoDown portamento_down = reinterpret_cast<const CommandPortamentoDown &>(command);
            const std::string command_value = from_portamento(portamento_down.channel, portamento_down.value);
            add_command("D", command_value);
            break;
        }
        case Instruction::SetMasterGainer: {
            const CommandSetMasterGainer set_master_gainer = reinterpret_cast<const CommandSetMasterGainer &>(command);
            const std::string command_value = from_gainer(set_master_gainer.gain);
            add_command("G", command_value);
            break;
        }
        case Instruction::SetBPM: {
            const CommandSetBPM set_bpm = reinterpret_cast<const CommandSetBPM &>(command);
            const std::string bpm_string = std::to_string(set_bpm.bpm);
            add_command("B", bpm_string);
            break;
        }
        case Instruction::SetDivision: {
            const CommandSetDivision set_division = reinterpret_cast<const CommandSetDivision &>(command);
            const std::string division_string = std::to_string(set_division.division);
            add_command("S", division_string);
            break;
        }
        case Instruction::ChangeByteValue:
        case Instruction::ChangeWordValue:
        case Instruction::ChangeDwordValue:
        case Instruction::ChangeFloatValue: {
            add_command("C");
            break;
        case Instruction::Empty: {
            add_command();
            break;
        }
        default: {
            throw std::runtime_error("Invalid command type");
        }
        }
        }

        indices.push_back(total_length);
        total_length += command.duration;
        for (size_t j = 1; j < command.duration; ++j) {
            add_command();
        }

        steps = total_length;
    }
}

std::vector<Command> CommandsPattern::to_command_vector() const {
    uint8_t duration = 1;

    std::vector<Command> command_vector;
    for (int i = steps - 1; i >= 0; --i) {
        if (i >= commands.size()) {
            if (command_vector.empty()) {
                Command empty_command;
                empty_command.duration = 1;
                command_vector.push_back(empty_command);
            }
            continue;
        }

        const std::string command = commands[i];
        const std::string value = values[i];

        if (command.empty() && i > 0) {
            ++duration;
            continue;
        }

        switch (command[0]) {
        case 'U': {
            CommandPortamentoUp portamento_up;
            portamento_up.duration = duration;
            split_portamento_value(value, portamento_up.channel, portamento_up.value);
            command_vector.push_back(reinterpret_cast<Command &>(portamento_up));
            break;
        }
        case 'D': {
            CommandPortamentoDown portamento_down;
            portamento_down.duration = duration;
            split_portamento_value(value, portamento_down.channel, portamento_down.value);
            command_vector.push_back(reinterpret_cast<Command &>(portamento_down));
            break;
        }
        case 'G': {
            CommandSetMasterGainer set_master_gainer;
            set_master_gainer.duration = duration;
            try {
                const double gain = std::round(std::stod(value) * UINT16_MAX / 2);
                set_master_gainer.gain = static_cast<uint16_t>(gain);
            } catch (const std::invalid_argument &) {
                set_master_gainer.gain = UINT16_MAX / 2;
            }

            command_vector.push_back(reinterpret_cast<Command &>(set_master_gainer));
            break;
        }
        case 'B': {
            CommandSetBPM set_bpm;
            set_bpm.duration = duration;
            try {
                int bpm = std::stoi(value);
                bpm = std::clamp(bpm, GUI_MIN_BPM, GUI_MAX_BPM);
                set_bpm.bpm = static_cast<uint16_t>(bpm);
            } catch (const std::invalid_argument &) {
                set_bpm.bpm = DEFAULT_BPM;
            }
            command_vector.push_back(reinterpret_cast<Command &>(set_bpm));
            break;
        }
        case 'S': {
            CommandSetDivision set_division;
            set_division.duration = duration;
            try {
                int division = std::stoi(value);
                division = std::clamp(division, GUI_MIN_DIVISION, GUI_MAX_DIVISION);
                set_division.division = static_cast<uint8_t>(division);
            } catch (const std::invalid_argument &) {
                set_division.division = DEFAULT_DIVISION;
            }
            command_vector.push_back(reinterpret_cast<Command &>(set_division));
            break;
        }
        default: {
            Command empty;
            empty.duration = duration;
            command_vector.push_back(empty);
            break;
        }
        }

        duration = 1;
    }

    std::reverse(command_vector.begin(), command_vector.end());
    return command_vector;
}

void CommandsPattern::to_buffer(const size_t sequence_index) const {
    if (sequence_index == -1) {
        return;
    }

    const CommandsSequence *sequence = commands_sequences[sequence_index];
    const size_t sequence_length = sequence->length;
    for (size_t i = 0; i < sequence_length; ++i) {
        const std::string command = commands[i];
        const std::string value = values[i];
        buffers.commands_sequences[sequence_index][i] = {command, value};
    }
}

void CommandsPattern::handle_input(const int min_row, const int max_row) {
    const bool valid = current_row >= 0 && current_row < commands.size();
    if (!valid) {
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
        selection = CommandSelection::Command;
        values_handler.clear();
        values_handler.capture_input = true;
    } else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
        selection = CommandSelection::Value;
        commands_handler.clear();
        commands_handler.capture_input = true;
    }

    if (selection == CommandSelection::Command) {
        if (commands_handler.handle_input()) {
            selection = CommandSelection::Value;
            values_handler.clear();
        }
    } else if (selection == CommandSelection::Value) {
        values_handler.handle_input();
    }
}

void CommandsPattern::set_selection(const int row, const CommandSelection item) {
    if (selection == CommandSelection::Value && (current_row != row || item != selection)) {
        values_handler.set_buffer(values[row]);
    }

    current_row = row;
    selection = item;
}

void CommandsPattern::add_command(const std::string &command, const std::string &value) {
    commands.push_back(command);
    values.push_back(value);
}

void CommandsPattern::split_portamento_value(const std::string &command_value, uint8_t &channel, uint16_t &value) {
    std::vector<std::string> value_parts = split(command_value, ',');
    channel = 0;
    value = 0;
    if (value_parts.size() >= 1 && !channels.empty()) {
        try {
            channel = std::stoi(value_parts[0]);
            channel = std::clamp(channel, static_cast<uint8_t>(0), static_cast<uint8_t>(channels.size() - 1));
        } catch (const std::invalid_argument &) {
        } catch (const std::out_of_range &) {
        }
    }

    if (value_parts.size() >= 2) {
        try {
            double parsed_value = std::stod(value_parts[1]);
            parsed_value = std::clamp(parsed_value, 0.0, MAX_PORTAMENTO);
            value = static_cast<uint16_t>(std::round(parsed_value * UINT16_MAX / MAX_PORTAMENTO));
        } catch (const std::invalid_argument &) {
        } catch (const std::out_of_range &) {
        }
    }
}

void CommandsPattern::split_change_value_parts(
    const std::string &command_value,
    TargetVariableType &target_variable_type,
    Target &target,
    uint8_t &index,
    uint16_t &offset
) {
    std::vector<std::string> value_parts = split(command_value, ',');
    size_t size = value_parts.size();
    target_variable_type = size < 1 ? TargetVariableType::Byte : static_cast<TargetVariableType>(std::stoi(value_parts[0]));
    target = size < 2 ? Target::DIRECT_OUTPUT : static_cast<Target>(std::stoi(value_parts[1]));
    index = size < 3 ? 0 : std::stoi(value_parts[2]);
    offset = size < 4 ? 0 : std::stoi(value_parts[3]);
}

double CommandsPattern::cast_portamento_to_double(const uint16_t value) {
    return MAX_PORTAMENTO * static_cast<float>(value) / UINT16_MAX;
}

std::string CommandsPattern::from_portamento(const uint8_t channel, const uint16_t value) {
    const double portamento_value = cast_portamento_to_double(value);
    return from_portamento(channel, portamento_value);
}

std::string CommandsPattern::from_portamento(const uint8_t channel, const double value) {
    const std::string channel_string = std::to_string(channel);
    const std::string value_string = convert_double_to_string(value, 3);
    return channel_string + "," + value_string;
}

std::string CommandsPattern::from_gainer(const uint16_t value) {
    const double gainer_value = 2 * static_cast<double>(value) / UINT16_MAX;
    return from_gainer(gainer_value);
}

std::string CommandsPattern::from_gainer(const double value) {
    return convert_double_to_string(value, 4);
}

void CommandsPattern::save_links(size_t sequence_index) const {
    if (sequence_index == -1) {
        return;
    }

    const CommandsSequence *sequence = commands_sequences[sequence_index];
    for (size_t i = 0; i < sequence->length; ++i) {
        const Command command = sequence->commands[i];
        if (command.instruction == static_cast<uint8_t>(Instruction::ChangeByteValue) ||
            command.instruction == static_cast<uint8_t>(Instruction::ChangeDwordValue) ||
            command.instruction == static_cast<uint8_t>(Instruction::ChangeWordValue) ||
            command.instruction == static_cast<uint8_t>(Instruction::ChangeFloatValue)) {
        }
    }
}
