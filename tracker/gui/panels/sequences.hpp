#ifndef GUI_SEQUENCES_HPP
#define GUI_SEQUENCES_HPP

#include <algorithm>
#include <string>
#include "../init.hpp"

class GUISequencesPanel {
  private:
    struct CurrentSequence {
        int steps;
        std::vector<uint8_t> pattern;
    } current_sequence;

    int sequence_index = 0;
    int selected_step = 0;
    int max_steps = 64;
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
                current_sequence.pattern.push_back(NOTE_OFF);
            }
        }
        current_sequence.steps = total_length;
    }

    void to_sequence() {
        Sequence *sequence = sequences[sequence_index];
        std::vector<Note> notes;
        uint16_t duration = 1;
        int16_t current_pitch = NOTE_REST;
        for (size_t i = 0; i < current_sequence.pattern.size(); ++i) {
            if (current_sequence.pattern[i] == NOTE_REST) {
                ++duration;
            } else {
                Note nute = {current_pitch, duration};
                notes.push_back(nute);
                duration = 1;
            }
        }
    }

    void update_sequences() {
        update_items(sequence_names, sequences.size(), "Sequence ", sequence_index);
    }

    void draw_sequence_length() {
        ImGui::PushID("##SequenceLength");
        ImGui::InputInt("Steps", &current_sequence.steps, 1, max_steps);
        ImGui::PopID();
        current_sequence.steps = std::clamp(current_sequence.steps, 1, 64);
    }

    void draw_sequence() {
        if (current_sequence.pattern.size() < static_cast<size_t>(current_sequence.steps)) {
            current_sequence.pattern.resize(current_sequence.steps, 255);
        } else if (current_sequence.pattern.size() > static_cast<size_t>(current_sequence.steps)) {
            current_sequence.pattern.resize(current_sequence.steps);
        }

        ImGui::Separator();
        ImGui::Text("Pattern:");

        ImGui::BeginChild("PatternScroll", ImVec2(0, 300), true);
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
            bool is_selected = (selected_step == i);
            if (ImGui::Selectable("##selectable", is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                selected_step = i;
            }
            ImGui::SameLine();
            std::string note_str = get_note_name(current_sequence.pattern[i]);
            if (is_selected) {
                ImGui::TextColored(ImVec4(1.0f, 0.2f, 1.0f, 1.0f), "%s", note_str.c_str());
            } else {
                ImGui::Text("%s", note_str.c_str());
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

#endif // GUI_SEQUENCES_HPP
