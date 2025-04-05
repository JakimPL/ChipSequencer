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

void draw_pattern(Pattern &pattern) {
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

    for (int i = 0; i < pattern.notes.size(); ++i) {
        ImGui::PushID(i);
        ImGui::Text("%d", i);
        ImGui::NextColumn();

        const bool is_selected = (pattern.current_row == i);
        if (ImGui::Selectable("##selectable", is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
            pattern.current_row = i;
        }
        ImGui::SameLine();

        const std::string note_string = get_note_name(pattern.notes[i]);
        if (is_selected) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 1.0f, 1.0f), "%s", note_string.c_str());
        } else {
            ImGui::Text("%s", note_string.c_str());
        }

        ImGui::NextColumn();
        const std::string octave_string = get_note_octave(pattern.notes[i]);
        if (is_selected) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 1.0f, 1.0f), "%s", octave_string.c_str());
        } else {
            ImGui::Text("%s", octave_string.c_str());
        }

        ImGui::NextColumn();
        ImGui::PopID();
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
