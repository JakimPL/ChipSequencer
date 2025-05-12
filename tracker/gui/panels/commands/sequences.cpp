#include "../../../general.hpp"
#include "../../../maps/commands.hpp"
#include "../../names.hpp"
#include "../../utils.hpp"
#include "sequences.hpp"

GUICommandsSequencesPanel::GUICommandsSequencesPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUICommandsSequencesPanel::draw() {
    ImGui::Begin("Commands sequences");
    ImGui::Columns(1, "commands_sequence_columns");

    std::vector<size_t> dependencies = song.find_commands_sequence_dependencies(sequence_index);
    push_tertiary_style();
    draw_add_or_remove("orders", dependencies);
    prepare_combo(commands_sequence_names, "##CommandsSequenceCombo", sequence_index);
    show_dependency_tooltip("orders", dependencies);
    pop_tertiary_style();

    ImGui::Separator();

    from();
    draw_sequence();
    draw_edit_dialog_box();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
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
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || !is_index_valid()) {
        return;
    }

    CommandsSequence *sequence = commands_sequences[sequence_index];
    current_sequence.pattern.to_buffer(sequence_index);
    const std::vector<Command> command_vector = current_sequence.pattern.to_command_vector();
    sequence->from_command_vector(command_vector);
}

void GUICommandsSequencesPanel::add() {
    CommandsSequence *new_sequence = song.add_commands_sequence();
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = commands_sequences.size() - 1;
    update();
}

void GUICommandsSequencesPanel::duplicate() {
    CommandsSequence *new_sequence = song.duplicate_commands_sequence(sequence_index);
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = commands_sequences.size() - 1;
    update();
}

void GUICommandsSequencesPanel::remove() {
    if (is_index_valid()) {
        song.remove_commands_sequence(sequence_index);
        sequence_index = std::max(0, sequence_index - 1);
        update();
    }
}

void GUICommandsSequencesPanel::update() {
    update_items(commands_sequence_names, commands_sequences.size(), "Commands sequence ", sequence_index);
    gui.update(GUIElement::Orders);
}

void GUICommandsSequencesPanel::draw_sequence_length() {
    const size_t old_size = current_sequence.pattern.steps;
    draw_number_of_items("Steps", "##SequenceLength", current_sequence.pattern.steps, 1, MAX_STEPS);

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

    if (!is_index_valid()) {
        ImGui::Text("No sequence available.");
        ImGui::Columns(1);
        return;
    }

    draw_sequence_length();

    if (current_sequence.pattern.steps > 0) {
        if (ImGui::Button("Edit")) {
            edit_dialog_box.visible = true;
        }
    }

    draw_commands_pattern(current_sequence.pattern);
}

void GUICommandsSequencesPanel::open_edit_dialog_box(const int item) {
    if (!is_index_valid()) {
        return;
    }

    edit_dialog_box.visible = true;
    edit_dialog_box.item = item;
    const std::string command = current_sequence.pattern.commands[item];
    edit_dialog_box.instruction = static_cast<int>(command.empty() ? Instruction::Empty : command_characters.at(command[0]));
}

void GUICommandsSequencesPanel::draw_edit_dialog_box() {
    if (!edit_dialog_box.visible) {
        return;
    }

    ImGui::SetNextWindowSize(ImVec2(450.0f, 250.0f), ImGuiCond_FirstUseEver);
    if (ImGui::Begin("Edit command", &edit_dialog_box.visible, ImGuiWindowFlags_NoCollapse)) {
        std::vector<std::string> names;
        for (const auto &[key, value] : instruction_names) {
            names.push_back(value);
        }

        ImGui::Text("Command:");
        push_tertiary_style();
        prepare_combo(names, "##EditCommand", edit_dialog_box.instruction);
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
            prepare_combo(channel_names, "##EditCommandChannel", edit_dialog_box.portamento_channel);
            draw_float_slider("Semitones", edit_dialog_box.portamento_value, {}, 0.0f, MAX_PORTAMENTO);
            break;
        }
        case Instruction::SetMasterGainer: {
            draw_float_slider("Gainer", edit_dialog_box.gainer, {}, 0.0f, 1.0f);
            break;
        }
        case Instruction::SetBPM: {
            draw_int_slider("BPM", edit_dialog_box.bpm, {}, GUI_MIN_BPM, GUI_MAX_BPM);
            break;
        }
        case Instruction::SetDivision: {
            draw_int_slider("Division", edit_dialog_box.division, {}, GUI_MIN_DIVISION, GUI_MAX_DIVISION);
            break;
        }
        case Instruction::ChangeByteValue:
        case Instruction::ChangeWordValue:
        case Instruction::ChangeDwordValue:
        case Instruction::ChangeFloatValue: {
            break;
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
            edit_dialog_box.visible = false;
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(button_width, 0))) {
            edit_dialog_box.visible = false;
            edit_dialog_box.item = -1;
        }
        ImGui::End();
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

void GUICommandsSequencesPanel::set_index(const int index) {
    sequence_index = clamp_index(index, commands_sequences.size());
}
