#include <sstream>

#include "../../general.hpp"
#include "../../maps/commands.hpp"
#include "../../maps/keys.hpp"
#include "../../utils/string.hpp"
#include "commands.hpp"

CommandsPattern::CommandsPattern()
    : commands_handler(std::vector<ImGuiKey>(std::begin(commands_keys), std::end(commands_keys))),
      values_handler(std::vector<ImGuiKey>(std::begin(values_keys), std::end(values_keys)), false) {
    commands_handler.set_limit(MAX_COMMAND_COMMAND_SIZE);
    values_handler.set_limit(MAX_COMMAND_VALUE_SIZE);
}

CommandsPattern::CommandsPattern(const uint8_t sequence_index)
    : CommandsPattern() {
    from_sequence(sequence_index);
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
        const Instruction instruction = static_cast<Instruction>(command.instruction);
        const std::string command_letter(1, command_letters.at(instruction));
        switch (instruction) {
        case Instruction::PortamentoUp: {
            const CommandPortamentoUp portamento_up = reinterpret_cast<const CommandPortamentoUp &>(command);
            const std::string command_value = from_portamento(portamento_up.channel, portamento_up.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::PortamentoDown: {
            const CommandPortamentoDown portamento_down = reinterpret_cast<const CommandPortamentoDown &>(command);
            const std::string command_value = from_portamento(portamento_down.channel, portamento_down.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::SetMasterGainer: {
            const CommandSetMasterGainer set_master_gainer = reinterpret_cast<const CommandSetMasterGainer &>(command);
            const std::string command_value = from_gainer(set_master_gainer.gain);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::SetBPM: {
            const CommandSetBPM set_bpm = reinterpret_cast<const CommandSetBPM &>(command);
            const std::string command_value = std::to_string(set_bpm.bpm);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::SetDivision: {
            const CommandSetDivision set_division = reinterpret_cast<const CommandSetDivision &>(command);
            const std::string command_value = std::to_string(set_division.division);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::ChangeByteValue: {
            const CommandChangeByteValue change_value = reinterpret_cast<const CommandChangeByteValue &>(command);
            const LinkKey key = get_command_key(reinterpret_cast<const CommandChangeValue *>(&change_value));
            const std::string command_value = from_change_value(TargetVariableType::Byte, key, change_value.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::ChangeWordValue: {
            const CommandChangeWordValue change_value = reinterpret_cast<const CommandChangeWordValue &>(command);
            const LinkKey key = get_command_key(reinterpret_cast<const CommandChangeValue *>(&change_value));
            const std::string command_value = from_change_value(TargetVariableType::Word, key, change_value.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::ChangeDwordValue: {
            const CommandChangeDwordValue change_value = reinterpret_cast<const CommandChangeDwordValue &>(command);
            const LinkKey key = get_command_key(reinterpret_cast<const CommandChangeValue *>(&change_value));
            const std::string command_value = from_change_value(TargetVariableType::Dword, key, change_value.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::ChangeFloatValue: {
            const CommandChangeFloatValue change_value = reinterpret_cast<const CommandChangeFloatValue &>(command);
            const LinkKey key = get_command_key(reinterpret_cast<const CommandChangeValue *>(&change_value));
            const std::string command_value = from_change_value(TargetVariableType::Float, key, change_value.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::AddByteValue: {
            const CommandAddByteValue add_value = reinterpret_cast<const CommandAddByteValue &>(command);
            const LinkKey key = get_command_key(reinterpret_cast<const CommandChangeValue *>(&add_value));
            const std::string command_value = from_change_value(TargetVariableType::Byte, key, add_value.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::AddWordValue: {
            const CommandAddWordValue add_value = reinterpret_cast<const CommandAddWordValue &>(command);
            const LinkKey key = get_command_key(reinterpret_cast<const CommandChangeValue *>(&add_value));
            const std::string command_value = from_change_value(TargetVariableType::Word, key, add_value.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::AddDwordValue: {
            const CommandAddDwordValue add_value = reinterpret_cast<const CommandAddDwordValue &>(command);
            const LinkKey key = get_command_key(reinterpret_cast<const CommandChangeValue *>(&add_value));
            const std::string command_value = from_change_value(TargetVariableType::Dword, key, add_value.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::AddFloatValue: {
            const CommandAddFloatValue add_value = reinterpret_cast<const CommandAddFloatValue &>(command);
            const LinkKey key = get_command_key(reinterpret_cast<const CommandChangeValue *>(&add_value));
            const std::string command_value = from_change_value(TargetVariableType::Float, key, add_value.value);
            add_command(command_letter, command_value);
            break;
        }
        case Instruction::Empty: {
            add_command();
            break;
        }
        case Instruction::Count:
        default: {
            throw std::runtime_error("Invalid command type: " + std::to_string(static_cast<int>(command.instruction)));
        }
        }

        indices.push_back(total_length);
        total_length += command.duration;
        durations.push_back(command.duration);
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

        const Instruction instruction = command.empty() ? Instruction::Empty : command_characters.at(command[0]);
        switch (instruction) {
        case Instruction::PortamentoUp: {
            CommandPortamentoUp portamento_up;
            portamento_up.duration = duration;
            split_portamento_value(value, portamento_up.channel, portamento_up.value);
            command_vector.push_back(reinterpret_cast<Command &>(portamento_up));
            break;
        }
        case Instruction::PortamentoDown: {
            CommandPortamentoDown portamento_down;
            portamento_down.duration = duration;
            split_portamento_value(value, portamento_down.channel, portamento_down.value);
            command_vector.push_back(reinterpret_cast<Command &>(portamento_down));
            break;
        }
        case Instruction::SetMasterGainer: {
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
        case Instruction::SetBPM: {
            CommandSetBPM set_bpm;
            set_bpm.duration = duration;
            const int bpm = string_to_integer(value, DEFAULT_BPM, GUI_MIN_BPM, GUI_MAX_BPM);
            set_bpm.bpm = static_cast<uint16_t>(bpm);
            command_vector.push_back(reinterpret_cast<Command &>(set_bpm));
            break;
        }
        case Instruction::SetDivision: {
            CommandSetDivision set_division;
            set_division.duration = duration;
            const int division = string_to_integer(value, DEFAULT_DIVISION, GUI_MIN_DIVISION, GUI_MAX_DIVISION);
            set_division.division = static_cast<uint8_t>(division);
            command_vector.push_back(reinterpret_cast<Command &>(set_division));
            break;
        }
        case Instruction::ChangeByteValue:
        case Instruction::ChangeWordValue:
        case Instruction::ChangeDwordValue:
        case Instruction::ChangeFloatValue:
        case Instruction::AddByteValue:
        case Instruction::AddWordValue:
        case Instruction::AddDwordValue:
        case Instruction::AddFloatValue: {
            const bool add = command[0] == command_letters.at(Instruction::AddByteValue);
            TargetVariableType target_variable_type;
            Target target;
            uint8_t index;
            uint16_t offset;
            uint32_t generic_value;
            split_change_value_parts(value, target_variable_type, target, index, offset, generic_value);
            const LinkKey key = {target, index, offset};
            const uint8_t id = -1;

            CommandChangeValue change_value;
            change_value.duration = duration;
            change_value.pointer = id;
            change_value.target = static_cast<uint8_t>(target);
            change_value.index = index;
            change_value.offset = offset;

            switch (target_variable_type) {
            case TargetVariableType::Byte: {
                CommandChangeByteValue change_byte_value = reinterpret_cast<CommandChangeByteValue &>(change_value);
                change_byte_value.instruction = add ? INSTRUCTION_ADD_BYTE_VALUE : INSTRUCTION_CHANGE_BYTE_VALUE;
                change_byte_value.value = static_cast<uint8_t>(generic_value);
                command_vector.push_back(reinterpret_cast<Command &>(change_byte_value));
                break;
            }
            case TargetVariableType::Word: {
                CommandChangeWordValue change_word_value = reinterpret_cast<CommandChangeWordValue &>(change_value);
                change_word_value.instruction = add ? INSTRUCTION_ADD_WORD_VALUE : INSTRUCTION_CHANGE_WORD_VALUE;
                change_word_value.value = static_cast<uint16_t>(generic_value);
                command_vector.push_back(reinterpret_cast<Command &>(change_word_value));
                break;
            }
            case TargetVariableType::Dword: {
                CommandChangeDwordValue change_dword_value = reinterpret_cast<CommandChangeDwordValue &>(change_value);
                change_dword_value.instruction = add ? INSTRUCTION_ADD_DWORD_VALUE : INSTRUCTION_CHANGE_DWORD_VALUE;
                change_dword_value.value = generic_value;
                command_vector.push_back(reinterpret_cast<Command &>(change_dword_value));
                break;
            }
            case TargetVariableType::Float: {
                CommandChangeFloatValue change_float_value = reinterpret_cast<CommandChangeFloatValue &>(change_value);
                change_float_value.instruction = add ? INSTRUCTION_ADD_FLOAT_VALUE : INSTRUCTION_CHANGE_FLOAT_VALUE;
                change_float_value.value = generic_value;
                command_vector.push_back(reinterpret_cast<Command &>(change_float_value));
                break;
            }
            case TargetVariableType::Count:
            default: {
                throw std::runtime_error("Invalid target variable type: " + std::to_string(static_cast<int>(target_variable_type)));
            }
            }

            break;
        }
        case Instruction::Empty: {
            Command empty;
            empty.duration = duration;
            command_vector.push_back(empty);
            break;
        }
        case Instruction::Count:
        default: {
            throw std::runtime_error("Invalid command type: " + std::to_string(static_cast<int>(instruction)));
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

int CommandsPattern::calculate_playing_row(size_t channel_index) {
    if (indices.empty()) {
        return -1;
    }

    size_t command_index = commands_sequence_current_command[channel_index] - 1;
    command_index = std::min(command_index, indices.size() - 1);
    int playing_row = indices[command_index];
    playing_row += durations[command_index];
    playing_row -= 1 + commands_sequence_timer[channel_index] / ticks_per_beat;
    return playing_row;
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
        channel = static_cast<uint8_t>(string_to_integer(value_parts[0], 0, 0, channels.size() - 1));
    }

    if (value_parts.size() >= 2) {
        const double parsed_value = string_to_double(value_parts[1], 0.0, 0.0, MAX_PORTAMENTO);
        value = static_cast<uint16_t>(std::round(parsed_value * UINT16_MAX / MAX_PORTAMENTO));
    }
}

void CommandsPattern::split_change_value_parts(
    const std::string &command_value,
    TargetVariableType &target_variable_type,
    Target &target,
    uint8_t &index,
    uint16_t &offset,
    uint32_t &value
) {
    std::vector<std::string> value_parts = split(command_value, ',');
    size_t size = value_parts.size();

    index = 0;
    offset = 0;
    target_variable_type = TargetVariableType::Byte;
    target = Target::DIRECT_OUTPUT;
    if (size >= 1) {
        const int variable_type_id = string_to_integer(value_parts[0], static_cast<int>(target_variable_type), 0, static_cast<int>(TargetVariableType::Count) - 1);
        target_variable_type = static_cast<TargetVariableType>(variable_type_id);
    }
    if (size >= 2) {
        const int target_id = string_to_integer(value_parts[1], static_cast<int>(target), 0, static_cast<int>(Target::COUNT) - 1);
        target = static_cast<Target>(target_id);
    }
    if (size >= 3) {
        int index_id = string_to_integer(value_parts[2], 0);

        switch (target) {
        case Target::DIRECT_OUTPUT:
        case Target::SPLITTER_OUTPUT: {
            index_id = std::clamp(index_id, 0, static_cast<int>(MAX_OUTPUT_CHANNELS - 1));
            break;
        }
        case Target::DSP:
        case Target::DIRECT_DSP:
        case Target::SPLITTER_DSP: {
            index_id = std::clamp(index_id, 0, static_cast<int>(dsps.size()) - 1);
            break;
        }
        case Target::CHANNEL: {
            index_id = std::clamp(index_id, 0, static_cast<int>(channels.size()) - 1);
            break;
        }
        case Target::COMMANDS_CHANNEL: {
            index_id = std::clamp(index_id, 0, static_cast<int>(commands_channels.size()) - 1);
            break;
        }
        case Target::ENVELOPE: {
            index_id = std::clamp(index_id, 0, static_cast<int>(envelopes.size()) - 1);
            break;
        }
        case Target::SEQUENCE: {
            index_id = std::clamp(index_id, 0, static_cast<int>(sequences.size()) - 1);
            break;
        }
        case Target::COMMANDS_SEQUENCE: {
            index_id = std::clamp(index_id, 0, static_cast<int>(commands_sequences.size()) - 1);
            break;
        }
        case Target::ORDER: {
            index_id = std::clamp(index_id, 0, static_cast<int>(orders.size()) - 1);
            break;
        }
        case Target::OSCILLATOR: {
            index_id = std::clamp(index_id, 0, static_cast<int>(oscillators.size()) - 1);
            break;
        }
        case Target::WAVETABLE: {
            index_id = std::clamp(index_id, 0, static_cast<int>(wavetables.size()) - 1);
            break;
        }
        case Target::COUNT:
        default: {
            throw std::runtime_error("Invalid target type: " + std::to_string(static_cast<int>(target)));
        }
        }
    }

    if (size >= 4) {
        offset = static_cast<uint16_t>(string_to_integer(value_parts[3], 0, 0, UINT16_MAX));
    }

    if (size >= 5) {
        switch (target_variable_type) {
        case TargetVariableType::Byte: {
            value = static_cast<uint8_t>(string_to_integer(value_parts[4], 0, 0, UINT8_MAX));
            break;
        }
        case TargetVariableType::Word: {
            value = static_cast<uint16_t>(string_to_integer(value_parts[4], 0, 0, UINT16_MAX));
            break;
        }
        case TargetVariableType::Dword: {
            value = static_cast<uint32_t>(string_to_integer(value_parts[4], 0, 0, UINT32_MAX));
            break;
        }
        case TargetVariableType::Float: {
            float numeric = string_to_double(value_parts[4], 0.0);
            value = reinterpret_cast<uint32_t &>(numeric);
            break;
        }
        case TargetVariableType::Count:
        default: {
            throw std::runtime_error("Invalid target variable type: " + std::to_string(static_cast<int>(target_variable_type)));
        }
        }
    }
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
    std::ostringstream stream;
    stream << channel_string << "," << value_string;
    return stream.str();
}

std::string CommandsPattern::from_gainer(const uint16_t value) {
    const double gainer_value = 2 * static_cast<double>(value) / UINT16_MAX;
    return from_gainer(gainer_value);
}

std::string CommandsPattern::from_gainer(const double value) {
    return convert_double_to_string(value, 4);
}

std::string CommandsPattern::from_change_value(const TargetVariableType type, const LinkKey key, const uint32_t value) {
    std::ostringstream stream;
    stream << static_cast<int>(type) << ","
           << static_cast<int>(key.target) << ","
           << key.index << ","
           << key.offset << ",";

    switch (type) {
    case TargetVariableType::Byte:
    case TargetVariableType::Word:
    case TargetVariableType::Dword: {
        stream << static_cast<int>(value);
        break;
    }
    case TargetVariableType::Float: {
        const float numeric = reinterpret_cast<const float &>(value);
        stream << convert_double_to_string(numeric);
        break;
    }
    case TargetVariableType::Count:
    default: {
        throw std::runtime_error("Invalid target variable type: " + std::to_string(static_cast<int>(type)));
    }
    }

    return stream.str();
}

std::string CommandsPattern::from_output_type(const OutputType &output_type, const int &value_integer, const float &value_float) {
    std::ostringstream stream;
    stream << static_cast<int>(output_type.variable_type) << ","
           << static_cast<int>(output_type.target) << ","
           << output_type.index << ","
           << output_type.offset << ",";

    switch (static_cast<TargetVariableType>(output_type.variable_type)) {
    case TargetVariableType::Byte:
    case TargetVariableType::Word:
    case TargetVariableType::Dword: {
        stream << static_cast<int>(value_integer);
        break;
    }
    case TargetVariableType::Float: {
        const float numeric = value_float;
        stream << convert_double_to_string(numeric);
        break;
    }
    case TargetVariableType::Count:
    default: {
        throw std::runtime_error("Invalid target variable type: " + std::to_string(static_cast<int>(output_type.variable_type)));
    }
    }

    return stream.str();
}

void CommandsPattern::save_links(size_t sequence_index) const {
    if (sequence_index == -1) {
        return;
    }

    std::vector<std::pair<size_t, LinkKey>> commands_to_update;

    commands_links[sequence_index].clear();
    CommandsSequence *sequence = commands_sequences[sequence_index];
    for (size_t i = 0; i < sequence->length; ++i) {
        const Command command = sequence->commands[i];
        if (command.instruction == static_cast<uint8_t>(Instruction::ChangeByteValue) ||
            command.instruction == static_cast<uint8_t>(Instruction::ChangeDwordValue) ||
            command.instruction == static_cast<uint8_t>(Instruction::ChangeWordValue) ||
            command.instruction == static_cast<uint8_t>(Instruction::ChangeFloatValue)) {
            CommandChangeValue change_value = reinterpret_cast<const CommandChangeValue &>(command);
            const uint16_t id = (sequence_index << 8) + i;
            const LinkKey key = {static_cast<Target>(change_value.target), change_value.index, change_value.offset};
            const Link link = {ItemType::COMMANDS, id, key.target, static_cast<uint8_t>(key.index), key.offset};
            commands_links[sequence_index][i] = link;
            commands_to_update.push_back({i, key});
        }
    }

    link_manager.set_links();

    for (auto &[index, key] : commands_to_update) {
        const size_t id = link_manager.find_pointer_id_by_key(key);
        CommandChangeValue &command = reinterpret_cast<CommandChangeValue &>(sequence->commands[index]);
        const LinkKey command_key = link_manager.get_pointer_and_key(id).second;
        command.target = static_cast<uint8_t>(command_key.target);
        command.index = command_key.index;
        command.offset = command_key.offset;
        command.pointer = static_cast<uint16_t>(id);
    }
}

LinkKey CommandsPattern::get_command_key(const CommandChangeValue *command) const {
    if (command->target == static_cast<uint8_t>(Target::COUNT)) {
        return link_manager.get_pointer_and_key(command->pointer).second;
    }

    return {static_cast<Target>(command->target), command->index, command->offset};
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
        if (commands_handler.handle_input(commands, current_row)) {
            selection = CommandSelection::Value;
            values_handler.clear();
        }
    } else if (selection == CommandSelection::Value) {
        values_handler.handle_input(values, current_row);
    }
}
