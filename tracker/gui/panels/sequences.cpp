#include <algorithm>

#include "../../general.hpp"
#include "../enums.hpp"
#include "../mapping.hpp"
#include "sequences.hpp"
#include "utils.hpp"

GUISequencesPanel::GUISequencesPanel() {
    from();
    update();
}

void GUISequencesPanel::draw() {
    ImGui::Begin("Sequence Editor");
    ImGui::Columns(1, "sequence_columns");

    draw_add_or_remove();
    prepare_combo(sequence_names, "##SequenceCombo", sequence_index);
    ImGui::Separator();

    from();
    draw_sequence();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

bool GUISequencesPanel::is_index_valid() const {
    return sequence_index >= 0 && sequence_index < sequences.size();
}

void GUISequencesPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    Sequence *sequence = sequences[sequence_index];
    uint16_t total_length = 0;

    current_sequence.pattern.clear();
    for (size_t i = 0; i < sequence->data_size / 2; ++i) {
        uint16_t duration = sequence->notes[i].duration;
        total_length += duration;
        current_sequence.pattern.push_back(sequence->notes[i].pitch);
        for (uint16_t j = 1; j < duration; ++j) {
            current_sequence.pattern.push_back(NOTE_REST);
        }
    }
    current_sequence.steps = total_length;
}

std::vector<Note> GUISequencesPanel::pattern_to_sequence() const {
    std::vector<Note> note_vector;

    uint8_t duration = 1;
    uint8_t pitch;
    for (int i = current_sequence.steps - 1; i >= 0; --i) {
        if (i >= current_sequence.pattern.size()) {
            if (note_vector.empty()) {
                note_vector.push_back({NOTE_REST, 1});
            }
            continue;
        }

        pitch = current_sequence.pattern[i];
        if (pitch == NOTE_REST && i > 0) {
            ++duration;
        } else {
            Note note = {pitch, duration};
            note_vector.push_back(note);
            duration = 1;
        }
    }

    std::reverse(note_vector.begin(), note_vector.end());
    return note_vector;
}

void GUISequencesPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    Sequence *sequence = sequences[sequence_index];
    const std::vector<Note> note_vector = pattern_to_sequence();

    const size_t length = note_vector.size();
    const uint8_t data_size = static_cast<uint8_t>(length * sizeof(Note));
    const size_t structure_size = data_size + 1;

    Sequence *new_sequence = static_cast<Sequence *>(operator new(structure_size));
    new_sequence->data_size = data_size;
    std::copy(note_vector.begin(), note_vector.end(), new_sequence->notes);

    sequences[sequence_index] = new_sequence;
    delete sequence;
}

void GUISequencesPanel::add() {
    Sequence *new_sequence = song.add_sequence();
    if (new_sequence == nullptr) {
        return;
    }

    sequence_index = sequences.size() - 1;
    update();
}

void GUISequencesPanel::remove() {
    if (is_index_valid()) {
        song.remove_sequence(sequence_index);
        sequence_index = std::max(0, sequence_index - 1);
        update();
    }
}

void GUISequencesPanel::update() {
    update_items(sequence_names, sequences.size(), "Sequence ", sequence_index);
    gui.update(GUIElement::Orders);
}

void GUISequencesPanel::draw_sequence_length() {
    draw_number_of_items("Steps", "##SequenceLength", current_sequence.steps, 1, max_steps);
}

void GUISequencesPanel::draw_sequence() {
    if (current_sequence.pattern.size() < static_cast<size_t>(current_sequence.steps)) {
        current_sequence.pattern.resize(current_sequence.steps, 255);
    } else if (current_sequence.pattern.size() > static_cast<size_t>(current_sequence.steps)) {
        current_sequence.pattern.resize(current_sequence.steps);
    }

    ImGui::Text("Pattern:");

    if (!is_index_valid()) {
        ImGui::Text("No sequence available.");
        ImGui::Columns(1);
        return;
    }

    draw_sequence_length();

    const float height = std::max(5.0f, ImGui::GetContentRegionAvail().y - 5.0f);
    ImGui::BeginChild("PatternScroll", ImVec2(0, height), true);
    ImGui::Columns(3, "pattern_columns", false);
    ImGui::SetColumnWidth(0, 50.0f);
    ImGui::SetColumnWidth(1, 75.0f);
    ImGui::SetColumnWidth(2, 75.0f);

    ImGui::Text("Index");
    ImGui::NextColumn();

    ImGui::Text("Note");
    ImGui::NextColumn();

    ImGui::Text("Octave");
    ImGui::NextColumn();

    ImGui::Separator();

    for (int i = 0; i < current_sequence.pattern.size(); ++i) {
        ImGui::PushID(i);
        ImGui::Text("%d", i);
        ImGui::NextColumn();

        const bool is_selected = (selected_step == i);
        if (ImGui::Selectable("##selectable", is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
            selected_step = i;
        }
        ImGui::SameLine();

        const std::string note_string = get_note_name(current_sequence.pattern[i]);
        if (is_selected) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 1.0f, 1.0f), "%s", note_string.c_str());
        } else {
            ImGui::Text("%s", note_string.c_str());
        }

        ImGui::NextColumn();
        const std::string octave_string = get_note_octave(current_sequence.pattern[i]);
        if (is_selected) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 1.0f, 1.0f), "%s", octave_string.c_str());
        } else {
            ImGui::Text("%s", octave_string.c_str());
        }

        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);
    ImGui::EndChild();
}

void GUISequencesPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    const bool valid = selected_step >= 0 && selected_step < current_sequence.pattern.size();
    if (!valid) {
        return;
    }

    const int edo = scale_composer.get_edo();
    if (edo == DEFAULT_EDO) {
        for (const auto &m : key_note_12_edo_mapping) {
            if (ImGui::IsKeyPressed(m.key)) {
                set_note(m.note_index, edo);
                break;
            }
        }
    } else {
        for (const auto &m : key_note_linear_mapping) {
            if (ImGui::IsKeyPressed(m.key)) {
                set_note(m.note_index, edo);
                break;
            }
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Space)) {
        current_sequence.pattern[selected_step] = NOTE_REST;
        jump();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Apostrophe) || ImGui::IsKeyPressed(ImGuiKey_Equal)) {
        current_sequence.pattern[selected_step] = NOTE_OFF;
        jump();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        current_sequence.pattern[selected_step] = NOTE_REST;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        selected_step = std::max(0, selected_step - 1);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        selected_step = std::min(current_sequence.steps - 1, selected_step + 1);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_KeypadAdd)) {
        current_sequence.steps = std::min(current_sequence.steps, max_steps);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_KeypadSubtract)) {
        current_sequence.steps = std::max(current_sequence.steps, 1);
    }
}

void GUISequencesPanel::jump() {
    selected_step = std::min(selected_step + gui.get_jump_step(), current_sequence.steps - 1);
}

void GUISequencesPanel::set_note(const int note_index, const int edo) {
    const int a4_index = frequency_table.get_a4_index();
    const int note = note_index + a4_index + edo * (gui.get_current_octave() - 5);
    if (note < 0 || note >= NOTES) {
        return;
    }
    current_sequence.pattern[selected_step] = note;
    jump();
}
