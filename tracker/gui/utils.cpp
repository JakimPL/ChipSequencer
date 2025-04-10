#include <algorithm>
#include <cmath>

#include "../general.hpp"
#include "../maps/routing.hpp"
#include "constants.hpp"
#include "mapping.hpp"
#include "names.hpp"
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

bool draw_button(const char *label, const float button_padding) {
    float text_width = ImGui::CalcTextSize(label).x;
    float button_width = text_width + button_padding;

    float full_width = ImGui::GetWindowContentRegionMax().x; // full available width in window
    ImGui::SetCursorPosX((full_width - button_width) * 0.5f);

    return ImGui::Button(label, ImVec2(button_width, 0));
}

std::pair<size_t, bool> draw_pattern(Pattern &pattern, const bool header, size_t index, const int playing_row, const uint16_t start, const uint16_t end) {
    bool select = false;
    const int min = std::max(static_cast<int>(start) - static_cast<int>(index), 0);
    const int max = std::min(static_cast<int>(end) - static_cast<int>(index), static_cast<int>(pattern.notes.size()));
    if (max <= 0 || min >= pattern.notes.size()) {
        return {index + pattern.notes.size(), false};
    }

    if (ImGui::BeginTable("PatternTable", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Note");
        ImGui::TableSetupColumn("Octave");
        if (header) {
            ImGui::TableHeadersRow();
        }

        for (int i = min; i < max; i++) {
            const int j = i + index;
            const bool is_selected = (pattern.current_row == i);
            ImGui::TableNextRow();
            if (playing_row == j) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, IM_COL32(128, 255, 128, 64));
            }

            if (is_selected) {
                ImGui::PushStyleColor(ImGuiCol_Text, GUI_HIGHLIGHT_COLOR);
            }

            ImGui::TableSetColumnIndex(0);
            const std::string index_string = std::to_string(j);
            if (ImGui::Selectable(index_string.c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
                pattern.current_row = i;
                select = true;
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", get_note_name(pattern.notes[i]).c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", get_note_octave(pattern.notes[i]).c_str());

            if (is_selected) {
                ImGui::PopStyleColor();
            }
        }

        ImGui::EndTable();
    }

    return {index + pattern.notes.size(), select};
}

bool draw_output(OutputType &output_type) {
    push_secondary_style();
    ImGui::Separator();
    ImGui::Text("Output:");
    const bool result = prepare_combo(target_types, "##OutputTargetCombo", output_type.target);
    switch (output_type.target) {
    case OUTPUT_TARGET_OUTPUT:
        draw_int_slider("Channel", output_type.output_channel, 0, song.get_output_channels() - 1);
        break;
    case OUTPUT_TARGET_DSP:
        if (dsps.empty()) {
            ImGui::Text("No DSPs available.");
            break;
        } else {
            draw_int_slider("DSP", output_type.dsp_channel, 0, dsps.size() - 1);
        }
    default:
        prepare_combo(parameter_types, "##OutputParameterCombo", output_type.parameter_type);
        const Target target = static_cast<Target>(output_type.parameter_type + OUTPUT_TARGET_PARAMETER);
        const auto &routing = routing_variables.at(target);
        switch (target) {
        case Target::ENVELOPE:
            int &variable_index = output_type.routing_item;
            bool value_changed = prepare_combo(envelope_names, "##OutputParameterEnvelopeCombo", output_type.index);
            value_changed |= prepare_combo(routing.labels, "##OutputParameterEnvelopeParameterCombo", variable_index);
            if (value_changed) {
                output_type.offset = routing.offsets[variable_index];
                output_type.variable_type = static_cast<int>(routing.types[variable_index]);
            }
            break;
        }
    }

    ImGui::Separator();
    ImGui::Checkbox("Additive", &output_type.additive);
    prepare_combo(variable_types, "##OutputTypeCombo", output_type.variable_type);
    ImGui::BeginDisabled(output_type.variable_type == 0);
    draw_int_slider("Shift", output_type.shift, 0, 15);
    ImGui::EndDisabled();
    pop_secondary_style();

    return result;
}

bool prepare_combo(const std::vector<std::string> &names, std::string label, int &index, const bool error_if_empty) {
    std::vector<const char *> names_cstr;
    for (const auto &name : names) {
        names_cstr.push_back(name.c_str());
    }

    float combo_width = ImGui::GetContentRegionAvail().x;
    ImGui::SetNextItemWidth(combo_width);

    if (error_if_empty && names.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Border, GUI_ERROR_COLOR);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
    }

    const bool result = ImGui::Combo(label.c_str(), &index, names_cstr.data(), names_cstr.size());

    if (error_if_empty && names.empty()) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    return result;
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

void push_secondary_style() {
    ImGui::PushStyleColor(ImGuiCol_CheckMark, GUI_SECONDARY_COLOR_LIGHT);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, GUI_SECONDARY_COLOR_LIGHT);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, GUI_SECONDARY_COLOR_LIGHT);
    ImGui::PushStyleColor(ImGuiCol_Button, GUI_SECONDARY_COLOR_LIGHT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GUI_SECONDARY_COLOR_BRIGHT);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, GUI_SECONDARY_COLOR);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, GUI_SECONDARY_COLOR_DARK);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GUI_SECONDARY_COLOR);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GUI_SECONDARY_COLOR);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, GUI_SECONDARY_COLOR_DARK);
    ImGui::PushStyleColor(ImGuiCol_Header, GUI_SECONDARY_COLOR_DARK);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, GUI_SECONDARY_COLOR);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, GUI_SECONDARY_COLOR);
}

void pop_secondary_style() {
    ImGui::PopStyleColor(13);
}

void push_tertiary_style() {
    ImGui::PushStyleColor(ImGuiCol_CheckMark, GUI_TERTIARY_COLOR_LIGHT);
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, GUI_TERTIARY_COLOR_LIGHT);
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, GUI_TERTIARY_COLOR_LIGHT);
    ImGui::PushStyleColor(ImGuiCol_Button, GUI_TERTIARY_COLOR_LIGHT);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, GUI_TERTIARY_COLOR_BRIGHT);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, GUI_TERTIARY_COLOR);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, GUI_TERTIARY_COLOR_DARK);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GUI_TERTIARY_COLOR);
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GUI_TERTIARY_COLOR);
    ImGui::PushStyleColor(ImGuiCol_PopupBg, GUI_TERTIARY_COLOR_DARK);
    ImGui::PushStyleColor(ImGuiCol_Header, GUI_TERTIARY_COLOR_DARK);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, GUI_TERTIARY_COLOR);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, GUI_TERTIARY_COLOR);
}

void pop_tertiary_style() {
    ImGui::PopStyleColor(13);
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
