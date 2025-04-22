#include <algorithm>
#include <cmath>

#include "../general.hpp"
#include "../maps/routing.hpp"
#include "../utils/string.hpp"
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

void draw_int_slider(const char *label, int &reference, const LinkKey key, int min, int max) {
    ImGui::BeginDisabled(link_manager.is_linked(key));

    const std::string slider_id = std::string("##") + label + "Slider";
    const std::string input_id = std::string("##") + label + "Input";
    ImGui::PushID(label);
    ImGui::SliderInt(slider_id.c_str(), &reference, min, max, label);
    draw_link_tooltip(key);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputInt(input_id.c_str(), &reference, 0, 0);
    draw_link_tooltip(key);
    ImGui::PopID();
    ImGui::EndDisabled();

    reference = std::clamp(reference, min, max);
}

void draw_float_slider(const char *label, float &reference, const LinkKey key, float min, float max, const GUIScale scale, const char *format) {
    ImGui::BeginDisabled(link_manager.is_linked(key));

    const std::string slider_id = std::string("##") + label + "Slider";
    const std::string input_id = std::string("##") + label + "Input";
    ImGui::PushID(label);

    float display_value = reference;
    const bool log_scale = scale == GUIScale::Logarithmic;
    if (log_scale) {
        display_value = std::log(reference / min) / std::log(max / min);
    }

    if (ImGui::SliderFloat(slider_id.c_str(), &display_value, log_scale ? 0.0f : min, log_scale ? 1.0f : max, label)) {
        switch (scale) {
        case GUIScale::Linear: {
            reference = display_value;
            break;
        }
        case GUIScale::Logarithmic: {
            reference = min * std::pow(max / min, display_value);
            break;
        }
        case GUIScale::Quadratic: {
            reference = min + (max - min) * display_value * display_value;
            break;
        }
        }
    }

    draw_link_tooltip(key);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputFloat(input_id.c_str(), &reference, 0.0f, 0.0f, format);
    draw_link_tooltip(key);
    ImGui::PopID();
    ImGui::EndDisabled();

    reference = std::clamp(reference, min, max);
}

void draw_knob(const char *label, float &reference, const LinkKey key, float min, float max) {
    ImGui::BeginDisabled(link_manager.is_linked(key));
    ImGui::PushID(label);
    ImGuiKnobs::Knob(label, &reference, min, max);
    draw_link_tooltip(key);
    ImGui::SameLine();
    ImGui::PopID();
    ImGui::EndDisabled();

    reference = std::clamp(reference, min, max);
}

void draw_link_tooltip(const LinkKey &key) {
    const std::vector<Link *> &links = link_manager.get_links(key);
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        if (links.empty()) {
            return;
        }

        std::ostringstream tooltip_stream;
        tooltip_stream << "Linked by ";
        for (size_t i = 0; i < links.size(); ++i) {
            const Link *link = links[i];
            const std::string name = link->type == ItemType::DSP ? dsp_names[link->id] : channel_names[link->id];
            tooltip_stream << name;
            if (i < links.size() - 1) {
                tooltip_stream << ", ";
            }
        }

        ImGui::SetTooltip("%s", tooltip_stream.str().c_str());
    }
}

void draw_popup(const std::string &message) {
    ImGui::Text("%s", message.c_str());
    const float button_width = GUI_BUTTON_WIDTH;
    const float window_width = ImGui::GetWindowSize().x;
    ImGui::SetCursorPosX((window_width - button_width) * 0.5f);
    if (ImGui::Button("Close", ImVec2(button_width, 0))) {
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
}

bool draw_button(const char *label, const float button_padding) {
    const float text_width = ImGui::CalcTextSize(label).x;
    const float button_width = text_width + button_padding;
    const float full_width = ImGui::GetWindowContentRegionMax().x;
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

    // realignment
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4.0f);

    return {index + pattern.notes.size(), select};
}

bool draw_output(OutputType &output_type, const int dsp_index) {
    push_secondary_style();
    ImGui::Separator();
    ImGui::Text("Output:");
    const bool value_changed = prepare_combo(target_types, "##OutputTargetCombo", output_type.target).value_changed;
    const size_t output_channels = song.get_output_channels();

    switch (static_cast<OutputTarget>(output_type.target)) {
    case OutputTarget::Splitter: {
        if (value_changed) {
            output_type.operation = static_cast<int>(OutputOperation::Add);
        }
        ImGui::Text("Splitter:");
        for (size_t i = 0; i < output_channels; ++i) {
            const std::string label = "Channel " + std::to_string(i);
            draw_float_slider(label.c_str(), output_type.splitter[i], {}, 0.0f, 1.0f);
        }

        break;
    }
    case OutputTarget::DirectOutput: {
        if (value_changed) {
            output_type.operation = static_cast<int>(OutputOperation::Add);
        }

        draw_int_slider("Channel", output_type.output_channel, {}, 0, output_channels - 1);
        break;
    }
    case OutputTarget::DSP: {
        if (dsps.empty() || dsp_index >= static_cast<int>(dsps.size()) - 1) {
            ImGui::Text("No DSPs available.");
            break;
        } else {
            if (value_changed) {
                output_type.operation = static_cast<int>(OutputOperation::Add);
            }

            draw_int_slider("DSP", output_type.dsp_channel, {}, dsp_index + 1, dsps.size() - 1);
            break;
        }
    }
    case OutputTarget::Parameter: {
        if (value_changed) {
            output_type.operation = static_cast<int>(OutputOperation::Set);
        }

        ImGui::Separator();
        prepare_combo(parameter_types, "##OutputParameterCombo", output_type.parameter_type);
        const Target target = static_cast<Target>(output_type.parameter_type + static_cast<int>(OutputTarget::Parameter));
        switch (target) {
        case Target::ENVELOPE: {
            draw_output_parameter(output_type, envelope_names, "Envelope");
            break;
        }
        case Target::SEQUENCE: {
            ImGui::Text("Not implemented yet.");
            break;
        }
        case Target::ORDER: {
            ImGui::Text("Not implemented yet.");
            break;
        }
        case Target::OSCILLATOR: {
            draw_output_parameter_oscillator(output_type);
            break;
        }
        case Target::WAVETABLE: {
            ImGui::Text("Not implemented yet.");
            break;
        }
        case Target::DSP: {
            draw_output_parameter_dsp(output_type);
            break;
        }
        case Target::CHANNEL: {
            draw_output_parameter(output_type, channel_names, "Channel");
            break;
        }
        case Target::SPLITTER:
        case Target::OUTPUT_CHANNEL:
        case Target::DSP_CHANNEL:
        case Target::UNUSED: {
            throw std::runtime_error("Invalid target type");
        }
        }
    }
    }

    ImGui::Separator();
    ImGui::Text("Operation:");
    prepare_combo(operation_names, "##OutputTypeOperation", output_type.operation);
    ImGui::Text("Variable:");
    prepare_combo(variable_types, "##OutputTypeCombo", output_type.variable_type);
    ImGui::BeginDisabled(output_type.variable_type == 0);
    draw_int_slider("Shift", output_type.shift, {}, 0, 15);
    ImGui::EndDisabled();
    pop_secondary_style();

    return value_changed;
}

void draw_output_parameter(OutputType &output_type, const std::vector<std::string> &names, const std::string label) {
    if (names.empty()) {
        const std::string text = "No " + to_lower(label) + "s available.";
        ImGui::Text("%s", text.c_str());
        return;
    }

    const Target target = static_cast<Target>(output_type.parameter_type + static_cast<int>(OutputTarget::Parameter));
    const RoutingItems &routing = routing_variables.at(target);
    int &item = output_type.routing_item;

    if (prepare_combo(names, "##OutputParameter" + label + "Combo", output_type.index, true).value_changed) {
        item = 0;
    }

    prepare_combo(routing.labels, "##OutputParameter" + label + "ParameterCombo", item);
    if (item < routing.labels.size()) {
        output_type.routing_index = item;
        output_type.offset = routing.offsets[item];
        output_type.variable_type = static_cast<int>(routing.types[item]);
    }
}

void draw_output_parameter_oscillator(OutputType &output_type) {
    if (oscillators.empty()) {
        ImGui::Text("No oscillator available.");
        return;
    }

    const RoutingItems &routing = routing_variables.at(Target::OSCILLATOR);
    const Oscillator *oscillator = static_cast<Oscillator *>(oscillators[output_type.index]);
    const auto [indices, labels, offsets, types] = routing.filter_items(oscillator->generator_index);
    int &item = output_type.routing_item;

    if (labels.empty()) {
        ImGui::Text("No parameters available.");
        return;
    }

    if (prepare_combo(oscillator_names, "##OutputParameterOscillatorCombo", output_type.index, true).value_changed) {
        item = 0;
    }

    prepare_combo(labels, "##OutputParameterOscillatorParameterCombo", item);
    if (item < labels.size()) {
        output_type.routing_index = indices[item];
        output_type.offset = routing.offsets[output_type.routing_index];
        output_type.variable_type = static_cast<int>(routing.types[output_type.routing_index]);
    }
}

void draw_output_parameter_dsp(OutputType &output_type) {
    if (dsps.empty()) {
        ImGui::Text("No DPS available.");
        return;
    }

    const RoutingItems &routing = routing_variables.at(Target::DSP);
    const DSP *dsp = static_cast<DSP *>(dsps[output_type.index]);
    const auto [indices, labels, offsets, types] = routing.filter_items(dsp->effect_index);
    int &item = output_type.routing_item;

    if (labels.empty()) {
        ImGui::Text("No parameters available.");
        return;
    }

    if (prepare_combo(dsp_names, "##OutputParameterDSPCombo", output_type.index, true).value_changed) {
        item = 0;
    }

    prepare_combo(labels, "##OutputParameterDSPParameterCombo", item);
    if (item < labels.size()) {
        output_type.routing_index = indices[item];
        output_type.offset = routing.offsets[output_type.routing_index];
        output_type.variable_type = static_cast<int>(routing.types[output_type.routing_index]);
    }
}

GUIState prepare_combo(const std::vector<std::string> &names, std::string label, int &index, const bool error_if_empty) {
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

    const bool value_changed = ImGui::Combo(label.c_str(), &index, names_cstr.data(), names_cstr.size());
    const bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);

    if (error_if_empty && names.empty()) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    return {value_changed, right_clicked};
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
