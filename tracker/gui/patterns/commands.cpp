#include "../../general.hpp"
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
          std::vector<ImGuiKey>(std::begin(values_keys), std::end(values_keys))
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
            const std::string channel_string = std::to_string(portamento_up.channel);
            const std::string value_string = std::to_string(portamento_up.value / UINT16_MAX);
            commands.push_back("U");
            values.push_back(channel_string + "," + value_string);
            break;
        }
        case Instruction::PortamentoDown: {
            const CommandPortamentoDown portamento_down = reinterpret_cast<const CommandPortamentoDown &>(command);
            const std::string channel_string = std::to_string(portamento_down.channel);
            const std::string value_string = std::to_string(portamento_down.value / UINT16_MAX);
            commands.push_back("D");
            values.push_back(channel_string + "," + value_string);
            break;
        }
        case Instruction::SetMasterGainer: {
            const CommandSetMasterGainer set_master_gainer = reinterpret_cast<const CommandSetMasterGainer &>(command);
            commands.push_back("G");
            values.push_back(std::to_string(set_master_gainer.gain / UINT16_MAX));
            break;
        }
        case Instruction::SetBPM: {
            const CommandSetBPM set_bpm = reinterpret_cast<const CommandSetBPM &>(command);
            const std::string bpm_string = std::to_string(set_bpm.bpm);
            commands.push_back("B");
            values.push_back(bpm_string);
            break;
        }
        case Instruction::SetDivision: {
            const CommandSetDivision set_division = reinterpret_cast<const CommandSetDivision &>(command);
            const std::string division_string = std::to_string(set_division.division);
            commands.push_back("S");
            values.push_back(division_string);
            break;
        }
        case Instruction::ChangeByteValue:
        case Instruction::ChangeWordValue:
        case Instruction::ChangeDwordValue:
        case Instruction::ChangeFloatValue: {
            commands.push_back("C");
            break;
        case Instruction::Empty: {
            commands.push_back("");
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
            commands.push_back("");
            values.push_back("");
        }

        steps = total_length;
    }

    if (commands.empty()) {
        commands.push_back("");
        values.push_back("");
        durations.push_back(1);
        indices.push_back(0);
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

            try {
                portamento_up.channel = std::stoi(value.substr(1));
            } catch (const std::out_of_range &e) {
                portamento_up.channel = 0;
            } catch (const std::invalid_argument &e) {
                portamento_up.channel = 0;
            }

            try {
                portamento_up.value = static_cast<uint16_t>(std::stoi(value.substr(3)) * UINT16_MAX);
            } catch (const std::out_of_range &e) {
                portamento_up.value = 0;
            } catch (const std::invalid_argument &e) {
                portamento_up.value = 0;
            }

            command_vector.push_back(reinterpret_cast<Command &>(portamento_up));
            break;
        }
        case 'D': {
            CommandPortamentoDown portamento_down;
            portamento_down.duration = duration;

            try {
                portamento_down.channel = std::stoi(value.substr(1));
            } catch (const std::out_of_range &e) {
                portamento_down.channel = 0;
            } catch (const std::invalid_argument &e) {
                portamento_down.channel = 0;
            }

            try {
                portamento_down.value = static_cast<uint16_t>(std::stoi(value.substr(3)) * UINT16_MAX);
            } catch (const std::out_of_range &e) {
                portamento_down.value = 0;
            } catch (const std::invalid_argument &e) {
                portamento_down.value = 0;
            }

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
        }
    } else if (selection == CommandSelection::Value) {
        values_handler.handle_input();
    }
}

void CommandsPattern::set_selection(const int row, const CommandSelection item) {
    current_row = row;
    selection = item;
}
