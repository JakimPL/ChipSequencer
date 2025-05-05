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
        const auto command = sequence->commands[i];
        const Command generic = std::get<Command>(command);
        const uint8_t command_type = generic.instruction;
        switch (static_cast<Instruction>(command_type)) {
        case Instruction::PortamentoUp: {
            const CommandPortamentoUp portamento_up = std::get<CommandPortamentoUp>(command);
            const std::string channel_string = std::to_string(portamento_up.channel);
            const std::string value_string = std::to_string(portamento_up.value / UINT16_MAX);
            commands.push_back("U");
            values.push_back(channel_string + "," + value_string);
            break;
        }
        case Instruction::PortamentoDown: {
            const CommandPortamentoDown portamento_down = std::get<CommandPortamentoDown>(command);
            const std::string channel_string = std::to_string(portamento_down.channel);
            const std::string value_string = std::to_string(portamento_down.value / UINT16_MAX);
            commands.push_back("D");
            values.push_back(channel_string + "," + value_string);
            break;
        }
        case Instruction::SetMasterGainer: {
            const CommandSetMasterGainer set_master_gainer = std::get<CommandSetMasterGainer>(command);
            commands.push_back("G");
            values.push_back(std::to_string(set_master_gainer.gain / UINT16_MAX));
            break;
        }
        case Instruction::SetBPM: {
            const CommandSetBPM set_bpm = std::get<CommandSetBPM>(command);
            const std::string bpm_string = std::to_string(set_bpm.bpm);
            commands.push_back("B");
            values.push_back(bpm_string);
            break;
        }
        case Instruction::SetDivision: {
            const CommandSetDivision set_division = std::get<CommandSetDivision>(command);
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
        case Instruction::Empty:
        default: {
            throw std::runtime_error("Invalid command type");
        }
        }
        }

        for (size_t j = 1; j < generic.duration; ++j) {
            commands.push_back("");
            values.push_back("");
        }

        steps = total_length;
    }
}

std::vector<CommandVariant> CommandsPattern::to_command_vector() const {
    std::vector<CommandVariant> command_vector;
    for (size_t i = 0; i < commands.size(); ++i) {
        Command command;
        command.instruction = static_cast<uint8_t>(Instruction::Empty);
        command.duration = durations[i];
        command_vector.push_back(command);
    }
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
