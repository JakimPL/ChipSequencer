#ifndef GUI_PANELS_SEQUENCES_HPP
#define GUI_PANELS_SEQUENCES_HPP

#include <algorithm>
#include <string>
#include "../init.hpp"
#include "utils.hpp"

class GUISequencesPanel {
  private:
    struct CurrentSequence {
        int steps;
        std::vector<int8_t> pattern;
    } current_sequence;

    int sequence_index = 0;
    int selected_step = 0;
    const int max_steps = 64;
    std::vector<std::string> sequence_names;

    void from_sequence() {
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

    std::vector<Note> pattern_to_sequence() {
        std::vector<Note> note_vector;

        uint8_t duration = 1;
        int8_t pitch;
        for (int i = current_sequence.pattern.size() - 1; i >= 0; --i) {
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

    void to_sequence() {
        Sequence *sequence = sequences[sequence_index];
        const std::vector<Note> note_vector = pattern_to_sequence();
        const size_t length = note_vector.size();
        const uint8_t data_size = static_cast<uint8_t>(length * sizeof(Note));
        const size_t structure_size = data_size + 1;

        Sequence *new_sequence = static_cast<Sequence *>(operator new(structure_size));
        new_sequence->data_size = data_size;
        std::copy(note_vector.begin(), note_vector.end(), new_sequence->notes);

        if (new_sequence->data_size > sequence->data_size) {
            for (size_t i = sequence->data_size / 2; i < new_sequence->data_size / 2; ++i) {
                new_sequence->notes[i].pitch = NOTE_REST;
                new_sequence->notes[i].duration = 1;
            }
        }

        sequences[sequence_index] = new_sequence;
    }

    void update_sequences() {
        update_items(sequence_names, sequences.size(), "Sequence ", sequence_index);
    }

    void draw_sequence_length() {
        draw_number_of_items("Steps", "##SequenceLength", current_sequence.steps, 1, max_steps);
    }

    void draw_sequence() {
        if (current_sequence.pattern.size() < static_cast<size_t>(current_sequence.steps)) {
            current_sequence.pattern.resize(current_sequence.steps, 255);
        } else if (current_sequence.pattern.size() > static_cast<size_t>(current_sequence.steps)) {
            current_sequence.pattern.resize(current_sequence.steps);
        }

        ImGui::Separator();
        ImGui::Text("Pattern:");

        const float height = std::max(5.0f, ImGui::GetContentRegionAvail().y - 5.0f);
        ImGui::BeginChild("PatternScroll", ImVec2(0, height), true);
        ImGui::Columns(2, "pattern_columns", false);
        ImGui::SetColumnWidth(0, 50.0f);

        ImGui::Text("Index");
        ImGui::NextColumn();
        ImGui::Text("Note");
        ImGui::NextColumn();

        ImGui::Separator();

        for (int i = 0; i < current_sequence.steps; ++i) {
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
            ImGui::PopID();
        }

        ImGui::Columns(1);
        ImGui::EndChild();
    }

  public:
    GUISequencesPanel() {
        update_sequences();
    }

    void draw() {
        ImGui::Begin("Sequence Editor");
        ImGui::Columns(1, "sequence_columns");

        prepare_combo(sequence_names, "##SequenceCombo", sequence_index);
        from_sequence();

        draw_sequence_length();
        draw_sequence();

        to_sequence();

        ImGui::Columns(1);
        ImGui::End();
    }
};

#endif // GUI_PANELS_SEQUENCES_HPP
