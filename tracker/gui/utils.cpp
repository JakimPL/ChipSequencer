#include <algorithm>
#include <cmath>

#include "../general.hpp"
#include "constants.hpp"
#include "mapping.hpp"
#include "utils.hpp"

int clamp_index(int index, const int size) {
    return std::clamp(index, 0, size - 1);
}

void draw_number_of_items(const std::string &label, const char *label_id, int &value, int min, int max, float label_length) {
    ImGui::PushID(label_id);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - label_length);
    ImGui::InputInt("Points", &value, min, max);
    ImGui::PopID();
    value = std::clamp(value, min, max);
}

void draw_int_slider(const char *label, int &reference, int min, int max) {
    const std::string slider_id = std::string("##") + label + "Slider";
    const std::string input_id = std::string("##") + label + "Input";
    ImGui::PushID(label);
    ImGui::SliderInt(slider_id.c_str(), &reference, min, max, label);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(input_id.c_str(), &reference, 0, 0);
    ImGui::PopID();
    reference = std::clamp(reference, min, max);
}

void draw_float_slider(const char *label, float &reference, float min, float max, bool log_scale) {
    const std::string slider_id = std::string("##") + label + "Slider";
    const std::string input_id = std::string("##") + label + "Input";
    ImGui::PushID(label);

    float display_value = reference;
    if (log_scale) {
        display_value = std::log(reference / min) / std::log(max / min);
    }

    if (ImGui::SliderFloat(slider_id.c_str(), &display_value, log_scale ? 0.0f : min, log_scale ? 1.0f : max, label)) {
        if (log_scale) {
            reference = min * std::pow(max / min, display_value);
        } else {
            reference = display_value;
        }
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputFloat(input_id.c_str(), &reference, 0.0f, 0.0f, "%.4f");
    ImGui::PopID();
    reference = std::clamp(reference, min, max);
}

void draw_knob(const char *label, float &reference, float min, float max) {
    ImGuiKnobs::Knob(label, &reference, min, max);
    ImGui::SameLine();
    reference = std::clamp(reference, min, max);
}

void draw_popup(const std::string &message) {
    ImGui::Text("%s", message.c_str());
    float buttonWidth = 60.0f;
    float windowWidth = ImGui::GetWindowSize().x;
    ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
    if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
}

void draw_button(const char *label, const std::function<void()> &callback, const float button_padding) {
    float text_width = ImGui::CalcTextSize(label).x;
    float button_width = text_width + button_padding;

    float full_width = ImGui::GetWindowContentRegionMax().x; // full available width in window
    ImGui::SetCursorPosX((full_width - button_width) * 0.5f);

    if (ImGui::Button(label, ImVec2(button_width, 0))) {
        callback();
    }
}

std::pair<size_t, bool> draw_pattern(Pattern &pattern, const bool header, size_t index, const int playing_row) {
    bool select = false;
    const float height = std::max(5.0f, ImGui::GetContentRegionAvail().y - 5.0f);
    const ImVec4 highlight_color = ImVec4(1.0f, 0.2f, 1.0f, 1.0f);

    ImGui::BeginChild("PatternScroll", ImVec2(0, height), true);
    if (ImGui::BeginTable("PatternTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Note");
        ImGui::TableSetupColumn("Octave");
        if (header) {
            ImGui::TableHeadersRow();
        }

        for (int i = 0; i < pattern.notes.size(); ++i) {
            const int j = i + index;
            const bool is_selected = (pattern.current_row == i);
            const std::string index_string = std::to_string(j);

            ImGui::TableNextRow();
            if (playing_row == j) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, IM_COL32(128, 255, 128, 64));
            }

            ImGui::TableSetColumnIndex(0);
            if (ImGui::Selectable(("##Selectable" + index_string).c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                pattern.current_row = i;
                select = true;
            }

            ImGui::SameLine();
            ImGui::TableSetColumnIndex(0);
            draw_colored_text(index_string, is_selected, highlight_color);

            ImGui::TableSetColumnIndex(1);
            const std::string note_string = get_note_name(pattern.notes[i]);
            draw_colored_text(note_string, is_selected, highlight_color);

            ImGui::TableSetColumnIndex(2);
            const std::string octave_string = get_note_octave(pattern.notes[i]);
            draw_colored_text(octave_string, is_selected, highlight_color);
        }

        ImGui::EndTable();
    }

    ImGui::Separator();
    ImGui::EndChild();

    return {index + pattern.notes.size(), select};
}

void draw_colored_text(const std::string &text, bool condition, const ImVec4 &color) {
    if (condition) {
        ImGui::TextColored(color, "%s", text.c_str());
    } else {
        ImGui::Text("%s", text.c_str());
    }
}

void prepare_combo(const std::vector<std::string> &names, std::string label, int &index) {
    std::vector<const char *> names_cstr;
    for (const auto &name : names) {
        names_cstr.push_back(name.c_str());
    }

    float combo_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(combo_width);
    ImGui::Combo(label.c_str(), &index, names_cstr.data(), names_cstr.size());
}

void update_items(std::vector<std::string> &names, size_t size, std::string label, int &index) {
    names.clear();
    for (size_t i = 0; i < size; ++i) {
        names.emplace_back(label + std::to_string(i));
    }
    if (index >= static_cast<int>(names.size())) {
        index = static_cast<int>(names.size()) - 1;
    }
    if (index < 0 && !names.empty()) {
        index = 0;
    }
}

std::string get_note_name(uint8_t note_value) {
    if (note_value == NOTE_REST) return "...";
    if (note_value == NOTE_OFF) return "===";
    if (note_value < NOTES) {
        return frequency_table.get_note_name(note_value);
    }
    return "???";
}

std::string get_note_octave(uint8_t note_value) {
    if (note_value == NOTE_REST) return "...";
    if (note_value == NOTE_OFF) return "===";
    if (note_value < NOTES) {
        return std::to_string(frequency_table.get_note_octave(note_value));
    }

    return 0;
}

uint8_t get_note_value(const std::string &note_name, const int octave) {
    if (note_name == "...") return NOTE_REST;
    if (note_name == "===") return NOTE_OFF;
    if (note_name == "???") return NOTES;
    return frequency_table.get_note_value(note_name, octave);
}
