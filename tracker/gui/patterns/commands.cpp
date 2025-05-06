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
            const double portamento_value = cast_portamento_to_double(portamento_up.value);
            const std::string channel_string = std::to_string(portamento_up.channel);
            const std::string value_string = convert_double_to_string(portamento_value, 3);
            add_command("U", channel_string + "," + value_string);
            break;
        }
        case Instruction::PortamentoDown: {
            const CommandPortamentoDown portamento_down = reinterpret_cast<const CommandPortamentoDown &>(command);
            const double portamento_value = cast_portamento_to_double(portamento_down.value);
            const std::string channel_string = std::to_string(portamento_down.channel);
            const std::string value_string = convert_double_to_string(portamento_value, 3);
            add_command("D", channel_string + "," + value_string);
            break;
        }
        case Instruction::SetMasterGainer: {
            const CommandSetMasterGainer set_master_gainer = reinterpret_cast<const CommandSetMasterGainer &>(command);
            add_command("G", std::to_string(set_master_gainer.gain / UINT16_MAX));
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
        // buffers.sequences[sequence_index][i] = {command, value};
    }
}

void CommandsPattern::handle_input(const int min_row, const int max_row) {
    const bool valid = current_row >= 0 && current_row < commands.size();
    if (!valid) {
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
        selection = CommandSelection::Command;
    } else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
        selection = CommandSelection::Value;
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

void CommandsPattern::split_portamento_value(const std::string &command_value, uint8_t &channel, uint16_t &value) const {
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

double CommandsPattern::cast_portamento_to_double(const uint16_t value) const {
    return MAX_PORTAMENTO * static_cast<float>(value) / UINT16_MAX;
}
