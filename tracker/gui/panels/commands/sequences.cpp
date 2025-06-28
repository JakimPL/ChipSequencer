#include <stdexcept>

#include "../../../general.hpp"
#include "../../../maps/commands.hpp"
#include "../../../structures/commands/command.hpp"
#include "../../names.hpp"
#include "../../utils.hpp"
#include "sequences.hpp"

GUICommandsSequencesPanel::GUICommandsSequencesPanel(const bool visible, const bool windowed)
    : GUIPanel("Commands sequences", visible, windowed) {
    initialize();
}

GUIElement GUICommandsSequencesPanel::get_element() const {
    return GUIElement::CommandsSequences;
}

void GUICommandsSequencesPanel::draw() {
    ImGui::BeginDisabled(lock_registry.is_locked(Target::COMMANDS_SEQUENCE, sequence_index));
    draw_sequence();
    ImGui::EndDisabled();
}

bool GUICommandsSequencesPanel::is_disabled() const {
    return gui.is_playing();
}

bool GUICommandsSequencesPanel::select_item() {
    std::vector<std::string> dependencies = song.find_commands_sequence_dependencies(sequence_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies);
    if (prepare_combo(this, commands_sequence_names, "##CommandsSequenceCombo", sequence_index, {}, false, GUI_COMBO_MARGIN_RIGHT).value_changed) {
        current_sequence.pattern.values_handler.clear();
        current_sequence.pattern.commands_handler.clear();
    }
    show_dependency_tooltip(dependencies);
    lock_item(Target::COMMANDS_SEQUENCE, sequence_index);
    pop_tertiary_style();
    ImGui::Separator();

    return !commands_sequences.empty();
}

void GUICommandsSequencesPanel::empty() {
    ImGui::Text("No commands sequences available.");
}

bool GUICommandsSequencesPanel::is_index_valid() const {
    return sequence_index >= 0 && sequence_index < commands_sequences.size();
}

void GUICommandsSequencesPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    current_sequence.pattern.from_sequence(sequence_index);
}

void GUICommandsSequencesPanel::to() const {
    const bool focus = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
    if (!is_index_valid()) {
        return;
    }

    if (save && gui.is_playing()) {
        gui.stop();
    }

    if (!save && !focus && !dialog_box_open) {
        return;
    }

    CommandsSequence *sequence = commands_sequences[sequence_index];
    current_sequence.pattern.to_buffer(sequence_index);
    const std::vector<Command> command_vector = current_sequence.pattern.to_command_vector();
    sequence->from_command_vector(command_vector);
    current_sequence.pattern.save_links(sequence_index);
}

void GUICommandsSequencesPanel::add() {
    CommandsSequence *new_sequence = song.add_commands_sequence();
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = commands_sequences.size() - 1;
    current_sequence.pattern.values_handler.clear();
    current_sequence.pattern.commands_handler.clear();
    perform_action_add(this, {Target::COMMANDS_SEQUENCE, sequence_index, 0});
    update();
}

void GUICommandsSequencesPanel::duplicate() {
    CommandsSequence *new_sequence = song.duplicate_commands_sequence(sequence_index);
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = commands_sequences.size() - 1;
    current_sequence.pattern.values_handler.clear();
    current_sequence.pattern.commands_handler.clear();
    update();
}

void GUICommandsSequencesPanel::remove() {
    const size_t previous_index = sequence_index;
    if (is_index_valid()) {
        perform_action_remove(this, {Target::COMMANDS_SEQUENCE, sequence_index, 0}, commands_sequences[sequence_index]);
        song.remove_commands_sequence(sequence_index);
        sequence_index = std::max(0, sequence_index - 1);
        update();
    }

    if (previous_index != sequence_index) {
        current_sequence.pattern.values_handler.clear();
        current_sequence.pattern.commands_handler.clear();
    }
}

void GUICommandsSequencesPanel::update() {
    sequence_index = clamp_index(sequence_index, commands_sequences.size());
    update_items(commands_sequence_names, commands_sequences.size(), "Commands sequence ", sequence_index);
    gui.update(GUIElement::Orders);
}

void GUICommandsSequencesPanel::shortcut_actions() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    switch (selection_action) {
    case PatternSelectionAction::SelectAll:
    case PatternSelectionAction::SelectChannel: {
        select_all();
        break;
    }
    case PatternSelectionAction::DeselectAll: {
        deselect_all();
        break;
    }
    case PatternSelectionAction::Delete: {
        delete_selection();
        break;
    }
    case PatternSelectionAction::SetNoteRest:
    case PatternSelectionAction::SetNoteCut:
    case PatternSelectionAction::SetNoteOff:
    case PatternSelectionAction::None:
    case PatternSelectionAction::TransposeUp:
    case PatternSelectionAction::TransposeDown:
    case PatternSelectionAction::TransposeOctaveUp:
    case PatternSelectionAction::TransposeOctaveDown:
    default: {
        break;
    }
    }

    selection_action = PatternSelectionAction::None;
}

void GUICommandsSequencesPanel::select_all() {
    selection.select(0, current_sequence.pattern.steps - 1, true);
}

void GUICommandsSequencesPanel::deselect_all() {
    selection.clear();
}

void GUICommandsSequencesPanel::delete_selection() {
    if (selection.is_active()) {
        PatternSelectionChange<CommandValue> changes;
        for (int row = selection.start; row <= selection.end; row++) {
            const CommandValue old_command_value = current_sequence.pattern.get_command(row);
            current_sequence.pattern.clear_row(row);
            const CommandValue new_command_value = current_sequence.pattern.get_command(row);
            changes[{0, 0, row}] = {old_command_value, new_command_value};
        }

        SetItemsFunction<CommandValue> function = [this](const std::map<PatternRow, CommandValue> &commands_values) {
            this->set_commands(commands_values);
        };
        perform_action_pattern_selection<CommandValue>(this, {Target::COMMANDS_SEQUENCE}, "Delete", changes, function);
    } else {
        const int row = current_sequence.pattern.current_row;
        current_sequence.pattern.clear_row(row);
    }
}

void GUICommandsSequencesPanel::draw_sequence_length() {
    const size_t old_size = current_sequence.pattern.steps;
    const LinkKey key = {Target::SPECIAL, sequence_index, SPECIAL_COMMANDS_SEQUENCE_LENGTH};
    draw_number_of_items(this, "Steps", "##SequenceLength", current_sequence.pattern.steps, 1, MAX_STEPS, key);

    if (old_size != current_sequence.pattern.steps) {
        current_sequence.pattern.commands.resize(current_sequence.pattern.steps);
        current_sequence.pattern.values.resize(current_sequence.pattern.steps);
        if (current_sequence.pattern.steps > old_size) {
            for (size_t i = old_size; i < current_sequence.pattern.steps; i++) {
                const auto &[command, value] = buffers.commands_sequences[sequence_index][i];
                current_sequence.pattern.commands[i] = command;
                current_sequence.pattern.values[i] = value;
            }
        }
    }
}

void GUICommandsSequencesPanel::draw_sequence() {
    if (current_sequence.pattern.commands.size() != static_cast<size_t>(current_sequence.pattern.steps)) {
        current_sequence.pattern.commands.resize(current_sequence.pattern.steps);
        current_sequence.pattern.values.resize(current_sequence.pattern.steps);
    }

    ImGui::Text("Pattern:");
    draw_sequence_length();
    if (current_sequence.pattern.steps > 0) {
        if (ImGui::Button("Edit")) {
            open_edit_dialog_box(current_sequence.pattern.current_row);
        }
    }

    SequenceRows secondary_sequence_rows;
    draw_commands_pattern(current_sequence.pattern, selection, secondary_sequence_rows, false);
}

void GUICommandsSequencesPanel::open_edit_dialog_box(const int item) {
    if (item < 0 || item >= current_sequence.pattern.steps) {
        return;
    }

    dialog_box_open = true;
    edit_dialog_box.visible = true;
    edit_dialog_box.item = item;
    const std::string command = current_sequence.pattern.commands[item];
    const Instruction instruction = command.empty() ? Instruction::Empty : command_characters.at(command[0]);
    edit_dialog_box.instruction = static_cast<int>(instruction);
    switch (instruction) {
    case Instruction::Empty: {
        break;
    }
    case Instruction::PortamentoUp:
    case Instruction::PortamentoDown: {
        uint8_t channel;
        uint16_t value;
        current_sequence.pattern.split_portamento_value(current_sequence.pattern.values[item], channel, value);
        edit_dialog_box.portamento_channel = channel;
        edit_dialog_box.portamento_value = CommandsPattern::cast_portamento_to_double(value);
        break;
    }
    case Instruction::SetMasterGainer: {
        edit_dialog_box.gainer = std::stod(current_sequence.pattern.values[item]);
        break;
    }
    case Instruction::SetBPM: {
        try {
            edit_dialog_box.bpm = std::stoi(current_sequence.pattern.values[item]);
        } catch (const std::invalid_argument &) {
            edit_dialog_box.bpm = DEFAULT_BPM;
        }
        break;
    }
    case Instruction::SetDivision: {
        try {
            edit_dialog_box.division = std::stoi(current_sequence.pattern.values[item]);
        } catch (const std::invalid_argument &) {
            edit_dialog_box.division = DEFAULT_DIVISION;
        }
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
        TargetVariableType target_variable_type;
        Target target;
        uint8_t index;
        uint16_t offset;
        uint32_t value;
        current_sequence.pattern.split_change_value_parts(
            current_sequence.pattern.values[item],
            target_variable_type,
            target,
            index,
            offset,
            value
        );

        edit_dialog_box.output_type.variable_type = static_cast<int>(target_variable_type);
        edit_dialog_box.output_type.target = static_cast<int>(target);
        edit_dialog_box.output_type.index = index;
        edit_dialog_box.output_type.offset = offset;

        if (is_instruction_float(instruction)) {
            edit_dialog_box.value_float = *reinterpret_cast<float *>(&value);
        } else {
            edit_dialog_box.value_integer = value;
        }

        if (is_instruction_add(instruction)) {
            edit_dialog_box.output_type.operation = static_cast<int>(OutputOperation::Add);
        } else {
            edit_dialog_box.output_type.operation = static_cast<int>(OutputOperation::Set);
        }
        break;
    }
    case Instruction::Count:
    default: {
        throw std::runtime_error("Invalid instruction: " + std::to_string(static_cast<int>(instruction)));
    }
    }
}

void GUICommandsSequencesPanel::draw_output_section() {
    push_secondary_style();
    ImGui::Text("Output:");

    draw_output_parameter(this, edit_dialog_box.output_type, {});
    ImGui::Text("Operation:");
    prepare_combo(this, simple_operation_names, "##OutputTypeOperation", edit_dialog_box.output_type.operation);
    ImGui::Text("Variable:");
    prepare_combo(this, variable_types, "##OutputTypeCombo", edit_dialog_box.output_type.variable_type);
    switch (static_cast<TargetVariableType>(edit_dialog_box.output_type.variable_type)) {
    case TargetVariableType::Byte: {
        draw_int_slider(this, "Value", edit_dialog_box.value_integer, {}, 0, UINT8_MAX);
        break;
    }
    case TargetVariableType::Word: {
        draw_int_slider(this, "Value", edit_dialog_box.value_integer, {}, 0, UINT16_MAX);
        break;
    }
    case TargetVariableType::Dword: {
        draw_int_slider(this, "Value", edit_dialog_box.value_integer, {}, 0, UINT32_MAX);
        break;
    }
    case TargetVariableType::Float: {
        draw_float_slider(this, "Value", edit_dialog_box.value_float, {}, 0.0f, 1.0f);
        break;
    }
    case TargetVariableType::Count:
    default:
        break;
    }
    pop_secondary_style();
}

void GUICommandsSequencesPanel::draw_dialog_box() {
    if (!edit_dialog_box.visible) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(450.0f, 250.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Edit command", &edit_dialog_box.visible, ImGuiWindowFlags_NoCollapse)) {
        std::vector<std::string> names;
        for (const auto &[key, value] : simple_instruction_names) {
            names.push_back(value);
        }

        edit_dialog_box.instruction = clamp_index(edit_dialog_box.instruction, names.size());

        ImGui::Text("Command:");
        push_tertiary_style();
        prepare_combo(this, names, "##EditCommand", edit_dialog_box.instruction);
        pop_tertiary_style();
        ImGui::Separator();

        push_secondary_style();
        switch (static_cast<Instruction>(edit_dialog_box.instruction)) {
        case Instruction::Empty: {
            break;
        }
        case Instruction::PortamentoUp:
        case Instruction::PortamentoDown: {
            ImGui::Text("Channel:");
            prepare_combo(this, channel_names, "##EditCommandChannel", edit_dialog_box.portamento_channel);
            draw_float_slider(this, "Semitones", edit_dialog_box.portamento_value, {}, 0.0f, MAX_PORTAMENTO);
            break;
        }
        case Instruction::SetMasterGainer: {
            draw_float_slider(this, "Gainer", edit_dialog_box.gainer, {}, 0.0f, 1.0f);
            break;
        }
        case Instruction::SetBPM: {
            draw_int_slider(this, "BPM", edit_dialog_box.bpm, {}, GUI_MIN_BPM, GUI_MAX_BPM);
            break;
        }
        case Instruction::SetDivision: {
            draw_int_slider(this, "Division", edit_dialog_box.division, {}, GUI_MIN_DIVISION, GUI_MAX_DIVISION);
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
            draw_output_section();
            break;
        }
        case Instruction::Count:
        default: {
            throw std::runtime_error("Invalid instruction: " + std::to_string(static_cast<int>(edit_dialog_box.instruction)));
        }
        }

        pop_secondary_style();
        ImGui::Separator();

        const float button_width = 75.0f;
        const float total_button_width = (button_width * 2) + ImGui::GetStyle().ItemSpacing.x;
        const float available_width = ImGui::GetContentRegionAvail().x;
        const float offset_x = (available_width - total_button_width) * 0.5f;

        if (offset_x > 0.0f) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
        }

        if (ImGui::Button("OK", ImVec2(button_width, 0))) {
            set_current_command();
            edit_dialog_box.visible = false;
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(button_width, 0))) {
            edit_dialog_box.visible = false;
        }

        ImGui::End();
    }
}

void GUICommandsSequencesPanel::set_current_command() {
    if (edit_dialog_box.item < 0 || edit_dialog_box.item >= current_sequence.pattern.steps) {
        return;
    }

    current_sequence.pattern.values_handler.clear();
    auto &command = current_sequence.pattern.commands[edit_dialog_box.item];
    auto &value = current_sequence.pattern.values[edit_dialog_box.item];
    command = command_letters.at(static_cast<Instruction>(edit_dialog_box.instruction));
    switch (static_cast<Instruction>(edit_dialog_box.instruction)) {
    case Instruction::Empty: {
        value = "";
        break;
    }
    case Instruction::PortamentoUp:
    case Instruction::PortamentoDown: {
        value = CommandsPattern::from_portamento(edit_dialog_box.portamento_channel, edit_dialog_box.portamento_value);
        break;
    }
    case Instruction::SetMasterGainer: {
        value = CommandsPattern::from_gainer(edit_dialog_box.gainer);
        break;
    }
    case Instruction::SetBPM: {
        value = std::to_string(edit_dialog_box.bpm);
        break;
    }
    case Instruction::SetDivision: {
        value = std::to_string(edit_dialog_box.division);
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
        command = edit_dialog_box.output_type.operation == static_cast<int>(OutputOperation::Add) ? command_letters.at(Instruction::AddFloatValue) : command_letters.at(Instruction::ChangeFloatValue);
        value = CommandsPattern::from_output_type(edit_dialog_box.output_type, edit_dialog_box.value_integer, edit_dialog_box.value_float);
        break;
    }
    case Instruction::Count:
    default: {
        throw std::runtime_error("Invalid instruction: " + std::to_string(static_cast<int>(edit_dialog_box.instruction)));
    }
    }
}

void GUICommandsSequencesPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    current_sequence.pattern.handle_input();
}

void GUICommandsSequencesPanel::set_commands(const std::map<PatternRow, CommandValue> &commands_values) {
    for (const auto &[row, command_value] : commands_values) {
        set_command(row, command_value);
    }
}

void GUICommandsSequencesPanel::set_command(const PatternRow &pattern_row, const std::string &command, const std::string &value) {
    set_command(pattern_row.row, command, value);
}

void GUICommandsSequencesPanel::set_command(const PatternRow &pattern_row, const CommandValue &command_value) {
    set_command(pattern_row, command_value.first, command_value.second);
}

void GUICommandsSequencesPanel::set_command(const int row, const std::string &command, const std::string &value) {
    current_sequence.pattern.set_command(row, command, value);
    current_sequence.pattern.current_row = row;
}

void GUICommandsSequencesPanel::set_index(const int index) {
    sequence_index = clamp_index(index, commands_sequences.size());
}

void GUICommandsSequencesPanel::clear_input_buffers() {
    current_sequence.pattern.commands_handler.clear();
    current_sequence.pattern.values_handler.clear();
}

bool GUICommandsSequencesPanel::is_active() const {
    return visible && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
}

void GUICommandsSequencesPanel::post_actions() {
    dialog_box_open = edit_dialog_box.visible;
    selection_action = PatternSelectionAction::None;
}

void GUICommandsSequencesPanel::register_shortcuts() {
    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectAll,
        [this]() {
            selection_action = PatternSelectionAction::SelectAll;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectNone,
        [this]() {
            selection_action = PatternSelectionAction::DeselectAll;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditDelete,
        [this]() {
            selection_action = PatternSelectionAction::Delete;
        }
    );
}