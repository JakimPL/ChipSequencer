#include <algorithm>
#include <cmath>

#include "../general.hpp"
#include "../maps/commands.hpp"
#include "../maps/keys.hpp"
#include "../maps/routing.hpp"
#include "../utils/math.hpp"
#include "../utils/string.hpp"
#include "constants.hpp"
#include "names.hpp"
#include "utils.hpp"
#include "history/actions/add.hpp"
#include "history/actions/note.hpp"
#include "history/actions/remove.hpp"
#include "history/actions/routing.hpp"
#include "history/actions/sequence.hpp"
#include "history/actions/text.hpp"
#include "history/actions/value.hpp"
#include "patterns/selection.hpp"

int clamp_index(int index, const int size) {
    return std::clamp(index, 0, size - 1);
}

void draw_number_of_items(GUIPanel *owner, const std::string &label, const char *label_id, int &value, int min, int max, const LinkKey key, float label_length) {
    const int old_value = value;

    ImGui::PushID(label_id);
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - label_length);
    const bool action = ImGui::InputInt("Items", &value, min, max);
    ImGui::PopID();

    value = std::clamp(value, min, max);
    if (action) {
        perform_action(owner, key, value, old_value);
    }
}

template <size_t n>
void draw_text(GUIPanel *owner, const char *label, char (&text)[n], const LinkKey key) {
    ImGui::BeginDisabled(link_manager.is_linked(key));

    const std::string old_value = text;

    ImGui::PushID(label);
    if (ImGui::InputText(label, text, IM_ARRAYSIZE(text))) {
        perform_action_string<n>(owner, key, text, old_value);
    }
    draw_link_tooltip(key);
    ImGui::PopID();
    ImGui::EndDisabled();
}

bool draw_checkbox(GUIPanel *owner, const char *label, bool &reference, const LinkKey key) {
    ImGui::BeginDisabled(link_manager.is_linked(key));

    const bool old_value = reference;

    ImGui::PushID(label);
    const bool action = ImGui::Checkbox(label, &reference);

    draw_link_tooltip(key);
    ImGui::PopID();
    ImGui::EndDisabled();

    if (action) {
        perform_action(owner, key, reference, old_value);
    }

    return action;
}

bool draw_int_slider(GUIPanel *owner, const char *label, int &reference, const LinkKey key, int min, int max) {
    ImGui::BeginDisabled(link_manager.is_linked(key));

    const int old_value = reference;

    const std::string slider_id = std::string("##") + label + "Slider";
    const std::string input_id = std::string("##") + label + "Input";
    ImGui::PushID(label);
    bool action = ImGui::SliderInt(slider_id.c_str(), &reference, min, max, label);
    draw_link_tooltip(key);

    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    action |= ImGui::InputInt(input_id.c_str(), &reference, 0, 0);
    draw_link_tooltip(key);

    ImGui::PopID();
    ImGui::EndDisabled();

    reference = std::clamp(reference, min, max);
    if (action) {
        perform_action(owner, key, reference, old_value);
    }

    return action;
}

bool draw_float_slider(GUIPanel *owner, const char *label, float &reference, const LinkKey key, float min, float max, const GUIScale scale, const char *format) {
    ImGui::BeginDisabled(link_manager.is_linked(key));

    const float old_value = reference;
    bool action = false;

    const std::string slider_id = std::string("##") + label + "Slider";
    const std::string input_id = std::string("##") + label + "Input";
    ImGui::PushID(label);

    float display_value;
    switch (scale) {
    case GUIScale::Logarithmic: {
        display_value = std::log(reference / min) / std::log(max / min);
        break;
    }
    case GUIScale::Quadratic: {
        display_value = std::sqrt((reference - min) / (max - min));
        break;
    }
    case GUIScale::Linear:
    default: {
        display_value = reference;
        break;
    }
    }

    const bool non_linear_scale = scale != GUIScale::Linear;
    if (ImGui::SliderFloat(slider_id.c_str(), &display_value, non_linear_scale ? 0.0f : min, non_linear_scale ? 1.0f : max, label)) {
        action = true;
        switch (scale) {
        case GUIScale::Logarithmic: {
            reference = min * std::pow(max / min, display_value);
            break;
        }
        case GUIScale::Quadratic: {
            reference = min + (max - min) * (display_value * display_value);
            break;
        }
        case GUIScale::Linear:
        default: {
            reference = display_value;
            break;
        }
        }
    }

    draw_link_tooltip(key);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    if (ImGui::InputFloat(input_id.c_str(), &reference, 0.0f, 0.0f, format)) {
        action = true;
    }

    draw_link_tooltip(key);
    ImGui::PopID();
    ImGui::EndDisabled();

    reference = std::clamp(reference, min, max);
    if (action) {
        perform_action_float(owner, key, reference, old_value, format);
    }

    return action;
}

bool draw_knob(GUIPanel *owner, const char *label, float &reference, const LinkKey key, float min, float max) {
    ImGui::BeginDisabled(link_manager.is_linked(key));

    const float old_value = reference;
    bool action = false;

    ImGui::PushID(label);
    action = ImGuiKnobs::Knob(label, &reference, min, max);
    draw_link_tooltip(key);
    ImGui::SameLine();
    ImGui::PopID();
    ImGui::EndDisabled();

    reference = std::clamp(reference, min, max);
    if (action) {
        perform_action_float(owner, key, reference, old_value);
    }

    return action;
}

void draw_link_tooltip(const LinkKey &key) {
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        const std::vector<Link *> &link_vector = link_manager.get_links(key);
        if (link_vector.empty()) {
            return;
        }

        std::set<std::string> item_names;
        for (const Link *link : link_vector) {
            std::string name;
            try {
                switch (link->type) {
                case ItemType::CHANNEL: {
                    name = channel_names.at(link->id);
                    break;
                }
                case ItemType::DSP: {
                    name = dsp_names.at(link->id);
                    break;
                }
                case ItemType::COMMANDS: {
                    const auto [sequence_index, channel_index] = LinkManager::unpack_command_id(link->id);
                    name = commands_sequence_names.at(sequence_index);
                    break;
                }
                case ItemType::COUNT:
                default:
                    throw std::out_of_range("Invalid link type: " + std::to_string(static_cast<int>(link->type)));
                }
            } catch (const std::out_of_range &) {
                name = "?";
            }

            if (!name.empty()) {
                item_names.insert(name);
            }
        }

        std::ostringstream tooltip_stream;
        tooltip_stream << "Linked by ";
        for (auto it = item_names.begin(); it != item_names.end(); ++it) {
            tooltip_stream << *it;
            if (std::next(it) != item_names.end()) {
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

void draw_confirmation_popup(
    const std::string &message,
    std::function<void()> ok_action,
    std::function<void()> save_action
) {
    ImGui::Text("%s", message.c_str());
    ImGui::Text("\n");
    ImGui::Separator();

    const float button_width = 80.0f;
    const float total_button_width = (button_width * 3) + ImGui::GetStyle().ItemSpacing.x * 2;
    const float available_width = ImGui::GetContentRegionAvail().x;
    const float offset_x = (available_width - total_button_width) * 0.5f;

    if (offset_x > 0.0f) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset_x);
    }

    if (ImGui::Button("OK", ImVec2(button_width, 0))) {
        if (ok_action) {
            ok_action();
        }
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Save", ImVec2(button_width, 0))) {
        if (save_action) {
            save_action();
        }
        if (ok_action) {
            ok_action();
        }
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(button_width, 0))) {
        ImGui::CloseCurrentPopup();
    }

    ImGui::SetItemDefaultFocus();
    ImGui::EndPopup();
}

bool draw_button(const char *label, const float button_padding) {
    const float text_width = ImGui::CalcTextSize(label).x;
    const float button_width = text_width + button_padding;
    const float full_width = ImGui::GetWindowContentRegionMax().x;
    ImGui::SetCursorPosX((full_width - button_width) * 0.5f);
    return ImGui::Button(label, ImVec2(button_width, 0));
}

std::pair<size_t, bool> draw_pattern(
    Pattern &pattern,
    PatternSelection &selection,
    SequenceRows &selected_rows,
    const bool pattern_view,
    const size_t channel_index,
    const bool header,
    const size_t index,
    const int playing_row,
    const uint16_t start,
    const uint16_t end,
    const RowDisplayStyle row_display,
    const bool locked
) {
    bool current = false;
    const int min_row_to_draw = std::max(static_cast<int>(start) - static_cast<int>(index), 0);
    const int max_row_to_draw = std::min(static_cast<int>(end) - static_cast<int>(index), static_cast<int>(pattern.notes.size()));

    if (max_row_to_draw <= 0 || min_row_to_draw >= static_cast<int>(pattern.notes.size())) {
        return {index + pattern.notes.size(), false};
    }

    ImGui::BeginDisabled(locked);
    if (ImGui::BeginTable("PatternTable", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)) {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("PatternContext");
        }

        ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Note");
        ImGui::TableSetupColumn("Octave");
        if (header) {
            ImGui::TableHeadersRow();
        }

        for (int i = min_row_to_draw; i < max_row_to_draw; i++) {
            const int j = i + index;
            const bool is_current = (pattern.current_row == i);
            const bool is_selected = !selection.command && selection.is_row_selected(channel_index, j);
            const bool is_secondary_selected = !selection.command && selected_rows.count({pattern.sequence_index, i}) > 0;

            ImGui::TableNextRow();

            if (playing_row == j) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, GUI_ROW_COLOR_PLAYING);
            } else if (is_selected) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, GUI_ROW_COLOR_SELECTED);
            } else if (is_secondary_selected) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, GUI_ROW_COLOR_SECONDARY_SELECTED);
            }

            if (is_current) {
                ImGui::PushStyleColor(ImGuiCol_Text, GUI_ROW_TEXT_CURRENT);
            }

            ImGui::TableSetColumnIndex(0);
            const std::string displayed_row = get_displayed_row(i, j, row_display);
            if (ImGui::Selectable(displayed_row.c_str(), is_current, ImGuiSelectableFlags_SpanAllColumns)) {
                pattern.current_row = i;
                current = true;
            }

            selection.form(false, channel_index, j);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", get_note_name(pattern.notes[i]).c_str());

            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", get_note_octave(pattern.notes[i]).c_str());

            if (is_current) {
                ImGui::PopStyleColor();
            }
        }

        if (ImGui::BeginPopup("PatternContext")) {
            draw_menu_item("Select all", ShortcutAction::PatternSelectAll);
            if (pattern_view) {
                draw_menu_item("Select channel", ShortcutAction::PatternSelectChannel);
            }
            draw_menu_item("Clear selection", ShortcutAction::PatternSelectNone);
            ImGui::Separator();
            draw_menu_item("Transpose +1", ShortcutAction::PatternTransposeUp);
            draw_menu_item("Transpose -1", ShortcutAction::PatternTransposeDown);
            draw_menu_item("Transpose octave +1", ShortcutAction::PatternTransposeOctaveUp);
            draw_menu_item("Transpose octave -1", ShortcutAction::PatternTransposeOctaveDown);
            ImGui::Separator();
            draw_menu_item("Set rest", ShortcutAction::PatternSetNoteRest);
            draw_menu_item("Set note cut", ShortcutAction::PatternSetNoteCut);
            draw_menu_item("Set note off", ShortcutAction::PatternSetNoteOff);
            ImGui::Separator();
            draw_menu_item("Cut", ShortcutAction::EditCut);
            draw_menu_item("Copy", ShortcutAction::EditCopy);
            draw_menu_item("Paste", ShortcutAction::EditPaste);
            draw_menu_item("Delete", ShortcutAction::EditDelete);
            ImGui::EndPopup();
        }

        ImGui::EndTable();
    }
    ImGui::EndDisabled();

    // realignment
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4.0f);

    return {index + pattern.notes.size(), current};
}

std::pair<size_t, bool> draw_commands_pattern(
    CommandsPattern &pattern,
    PatternSelection &selection,
    SequenceRows &selected_rows,
    const bool pattern_view,
    const size_t channel_index,
    const bool header,
    const size_t index,
    const int playing_row,
    const uint16_t start,
    const uint16_t end,
    const RowDisplayStyle row_display,
    const bool locked
) {
    bool current = false;
    const int min = std::max(static_cast<int>(start) - static_cast<int>(index), 0);
    const int max = std::min(static_cast<int>(end) - static_cast<int>(index), static_cast<int>(pattern.commands.size()));
    if (max <= 0 || min >= pattern.commands.size()) {
        return {index + pattern.commands.size(), false};
    }

    ImGui::BeginDisabled(locked);
    if (ImGui::BeginTable("CommandsPatternTable", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg)) {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
            ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopup("CommandsPatternContext");
        }

        ImGui::BeginDisabled(gui.is_playing());
        ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 40.0f);
        ImGui::TableSetupColumn("Com", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Value");
        if (header) {
            ImGui::TableHeadersRow();
        }

        for (int i = min; i < max; i++) {
            const int j = i + index;
            const bool is_command_current = (pattern.current_row == i && pattern.selection == CommandSelection::Command);
            const bool is_value_current = (pattern.current_row == i && pattern.selection == CommandSelection::Value);
            const bool is_selected = selection.command && selection.is_row_selected(channel_index, j);
            const bool is_secondary_selected = selection.command && selected_rows.count({pattern.sequence_index, i}) > 0;

            ImGui::TableNextRow();
            if (playing_row == j) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, GUI_ROW_COLOR_PLAYING);
            } else if (is_selected) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, GUI_ROW_COLOR_SELECTED);
            } else if (is_secondary_selected) {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, GUI_ROW_COLOR_SECONDARY_SELECTED);
            }

            if (is_command_current) {
                ImGui::PushStyleColor(ImGuiCol_Text, GUI_ROW_TEXT_CURRENT);
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, GUI_ROW_COLOR_CURRENT_COMMAND);
            } else if (is_value_current) {
                ImGui::PushStyleColor(ImGuiCol_Text, GUI_ROW_TEXT_CURRENT);
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, GUI_ROW_COLOR_CURRENT_VALUE);
            }

            ImGui::TableSetColumnIndex(0);
            const std::string displayed_row = get_displayed_row(i, j, row_display);
            if (ImGui::Selectable(displayed_row.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
            }
            selection.form(true, channel_index, j);
            ImGui::SameLine(0, 0);

            if (is_command_current || is_value_current) {
                ImGui::PopStyleColor();
            }

            if (is_command_current) {
                ImGui::PushStyleColor(ImGuiCol_Text, GUI_ROW_TEXT_CURRENT);
            }

            ImGui::PushID(i);
            ImGui::TableSetColumnIndex(1);
            const std::string command = pattern.commands[i].empty() ? "." : pattern.commands[i];
            if (ImGui::Selectable(command.c_str(), is_command_current, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
                pattern.set_selection(i, CommandSelection::Command);
                current = true;
            }

            show_commands_pattern_tooltip(pattern, i);
            ImGui::PopID();

            if (is_command_current) {
                ImGui::PopStyleColor();
            }

            std::string value;
            if (is_value_current) {
                ImGui::PushStyleColor(ImGuiCol_Text, GUI_ROW_TEXT_CURRENT);
                value = pattern.values_handler.get_buffer();
            } else {
                value = pattern.values[i].empty() ? "" : pattern.values[i];
            }

            pad_string(value, '.', MAX_COMMAND_VALUE_SIZE);

            ImGui::TableSetColumnIndex(2);
            ImGui::PushID(MAX_STEPS + i);
            if (ImGui::Selectable(value.c_str(), is_value_current)) {
                pattern.set_selection(i, CommandSelection::Value);
                pattern.values_handler.set_buffer(pattern.values[i]);
                current = true;
            }
            show_commands_pattern_tooltip(pattern, i);
            ImGui::PopID();

            if (is_value_current) {
                ImGui::PopStyleColor();
            }
        }

        if (ImGui::BeginPopup("CommandsPatternContext")) {
            draw_menu_item("Select all", ShortcutAction::PatternSelectAll);
            if (pattern_view) {
                draw_menu_item("Select channel", ShortcutAction::PatternSelectChannel);
            }
            draw_menu_item("Clear selection", ShortcutAction::PatternSelectNone);
            ImGui::Separator();
            draw_menu_item("Cut", ShortcutAction::EditCut);
            draw_menu_item("Copy", ShortcutAction::EditCopy);
            draw_menu_item("Paste", ShortcutAction::EditPaste);
            draw_menu_item("Delete", ShortcutAction::EditDelete);
            ImGui::EndPopup();
        }

        ImGui::EndDisabled();
        ImGui::EndTable();
    }
    ImGui::EndDisabled();

    // realignment
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 4.0f);

    return {index + pattern.commands.size(), current};
}

void draw_output_output_splitter(GUIPanel *owner, OutputType &output_type, const LinkKey key) {
    const size_t output_channels = song.get_output_channels();
    ImGui::Text("Splitter:");
    for (size_t i = 0; i < output_channels; ++i) {
        const LinkKey splitter_key = {key.target, key.index, static_cast<uint16_t>(key.offset + i)};
        const std::string label = "Channel " + std::to_string(i);
        draw_float_slider(owner, label.c_str(), output_type.splitter[i], splitter_key, 0.0f, 1.0f);
    }
}

void draw_output_dsp_splitter(GUIPanel *owner, OutputType &output_type, const int dsp_index, const LinkKey key) {
    if (dsps.empty() || dsp_index >= static_cast<int>(dsps.size()) - 1) {
        ImGui::Text("No DSP available.");
        return;
    }

    ImGui::Text("Initial DSP:");
    draw_int_slider(owner, "DSP", output_type.dsp_channel, {Target::SPECIAL, dsp_index, SPECIAL_DSP_INDEX}, dsp_index + 1, dsps.size() - 1);
    ImGui::Text("Splitter:");
    int start = output_type.dsp_channel;
    int end = start + std::clamp(static_cast<int>(dsps.size()) - start, 0, MAX_OUTPUT_CHANNELS);
    for (int i = output_type.dsp_channel; i < end; ++i) {
        const size_t j = i - output_type.dsp_channel;
        const std::string label = dsp_names[i];
        const LinkKey splitter_key = {key.target, key.index, static_cast<uint16_t>(key.offset + j)};
        draw_float_slider(owner, label.c_str(), output_type.splitter[j], splitter_key, 0.0f, 1.0f);
    }
}

void draw_output_direct_output(GUIPanel *owner, OutputType &output_type, const LinkKey key) {
    const size_t output_channels = song.get_output_channels();
    draw_int_slider(owner, "Channel", output_type.output_channel, {Target::SPECIAL, key.index, SPECIAL_CHANNEL_INDEX}, 0, output_channels - 1);
}

void draw_output_direct_dsp(GUIPanel *owner, OutputType &output_type, const int dsp_index, const LinkKey key) {
    if (dsps.empty() || dsp_index >= static_cast<int>(dsps.size()) - 1) {
        ImGui::Text("No DSP available.");
        return;
    }

    draw_int_slider(owner, "DSP", output_type.dsp_channel, {Target::SPECIAL, dsp_index, SPECIAL_DSP_INDEX}, dsp_index + 1, dsps.size() - 1);
}

bool draw_output_parameter(GUIPanel *owner, OutputType &output_type, const LinkKey key) {
    ImGui::Separator();
    bool value_changed = prepare_combo(owner, parameter_types, "##OutputParameterCombo", output_type.parameter_type).value_changed;
    output_type.target = output_type.parameter_type + static_cast<int>(OutputTarget::Parameter);
    switch (static_cast<Target>(output_type.target)) {
    case Target::ENVELOPE: {
        draw_output_parameter_generic(owner, output_type, envelope_names, "Envelope");
        break;
    }
    case Target::SEQUENCE:
    case Target::COMMANDS_SEQUENCE:
    case Target::ORDER:
    case Target::WAVETABLE:
    case Target::COMMANDS_CHANNEL: {
        ImGui::Text("Not implemented yet.");
        break;
    }
    case Target::OSCILLATOR: {
        draw_output_parameter_oscillator(owner, output_type);
        break;
    }
    case Target::DSP: {
        draw_output_parameter_dsp(owner, output_type);
        break;
    }
    case Target::CHANNEL: {
        draw_output_parameter_generic(owner, output_type, channel_names, "Channel");
        break;
    }
    case Target::SPLITTER_OUTPUT:
    case Target::SPLITTER_DSP:
    case Target::DIRECT_OUTPUT:
    case Target::DIRECT_DSP:
    case Target::SPECIAL:
    case Target::COUNT:
    default: {
        throw std::runtime_error("Invalid target type: " + std::to_string(output_type.target));
    }
    }

    return value_changed;
}

void draw_output_parameter_generic(GUIPanel *owner, OutputType &output_type, const std::vector<std::string> &names, const std::string label) {
    if (names.empty()) {
        const std::string text = "No " + to_lower(label) + "s available.";
        ImGui::Text("%s", text.c_str());
        return;
    }

    const Target target = static_cast<Target>(output_type.parameter_type + static_cast<int>(OutputTarget::Parameter));
    const RoutingItems &routing = routing_variables.at(target);
    int &item = output_type.routing_item;

    if (prepare_combo(owner, names, "##OutputParameter" + label + "Combo", output_type.index, {Target::SPECIAL, 0, SPECIAL_OUTPUT_PARAMETER_INDEX}, true).value_changed) {
        item = 0;
    }

    prepare_combo(owner, routing.labels, "##OutputParameter" + label + "ParameterCombo", item, {Target::SPECIAL, 0, SPECIAL_OUTPUT_ROUTING_ITEM});
    if (item >= routing.labels.size()) {
        item = 0;
    }

    output_type.routing_index = item;
    output_type.offset = routing.offsets[item];
    output_type.variable_type = static_cast<int>(routing.types[item]);
}

void draw_output_parameter_oscillator(GUIPanel *owner, OutputType &output_type) {
    if (oscillators.empty()) {
        ImGui::Text("No oscillator available.");
        return;
    }

    const RoutingItems &routing = routing_variables.at(Target::OSCILLATOR);
    int &item = output_type.routing_item;

    if (prepare_combo(owner, oscillator_names, "##OutputParameterOscillatorCombo", output_type.index, {Target::SPECIAL, 0, SPECIAL_OUTPUT_PARAMETER_INDEX}, true).value_changed) {
        item = 0;
    }

    output_type.index = clamp_index(output_type.index, static_cast<int>(oscillators.size()));
    const Oscillator *oscillator = reinterpret_cast<Oscillator *>(oscillators[output_type.index]);
    const auto [indices, labels, offsets, types] = routing.filter_items(oscillator->generator_index);
    if (labels.empty()) {
        ImGui::Text("No parameters available.");
        return;
    }

    prepare_combo(owner, labels, "##OutputParameterOscillatorParameterCombo", item, {Target::SPECIAL, 0, SPECIAL_OUTPUT_ROUTING_ITEM});
    if (item >= labels.size()) {
        item = 0;
    }

    output_type.routing_index = indices[item];
    output_type.offset = routing.offsets[output_type.routing_index];
    output_type.variable_type = static_cast<int>(routing.types[output_type.routing_index]);
}

void draw_output_parameter_dsp(GUIPanel *owner, OutputType &output_type) {
    if (dsps.empty()) {
        ImGui::Text("No DPS available.");
        return;
    }

    const RoutingItems &routing = routing_variables.at(Target::DSP);
    int &item = output_type.routing_item;

    if (prepare_combo(owner, dsp_names, "##OutputParameterDSPCombo", output_type.index, {Target::SPECIAL, 0, SPECIAL_OUTPUT_PARAMETER_INDEX}, true).value_changed) {
        item = 0;
    }

    output_type.index = clamp_index(output_type.index, static_cast<int>(dsps.size()));
    const DSP *dsp = static_cast<DSP *>(dsps[output_type.index]);
    const auto [indices, labels, offsets, types] = routing.filter_items(dsp->effect_index);

    if (labels.empty()) {
        ImGui::Text("No parameters available.");
        return;
    }

    prepare_combo(owner, labels, "##OutputParameterDSPParameterCombo", item, {Target::SPECIAL, 0, SPECIAL_OUTPUT_ROUTING_ITEM});
    if (item >= labels.size()) {
        item = 0;
    }

    output_type.routing_index = indices[item];
    output_type.offset = routing.offsets[output_type.routing_index];
    output_type.variable_type = static_cast<int>(routing.types[output_type.routing_index]);
}

bool draw_output(GUIPanel *owner, OutputType &output_type, const LinkKey key) {
    push_secondary_style();
    ImGui::Separator();
    ImGui::Text("Output:");
    bool value_changed = prepare_combo(owner, target_types, "##OutputTargetCombo", output_type.target, {Target::SPECIAL, 0, SPECIAL_OUTPUT_TARGET}).value_changed;
    const int dsp_index = key.target == Target::DSP ? key.index : -1;

    switch (static_cast<OutputTarget>(output_type.target)) {
    case OutputTarget::OutputSplitter: {
        draw_output_output_splitter(owner, output_type, key);

        if (value_changed) {
            output_type.operation = static_cast<int>(OutputOperation::Add);
        }

        break;
    }
    case OutputTarget::DSPSplitter: {
        draw_output_dsp_splitter(owner, output_type, dsp_index, key);

        if (value_changed) {
            output_type.operation = static_cast<int>(OutputOperation::Add);
        }

        break;
    }
    case OutputTarget::DirectOutput: {
        draw_output_direct_output(owner, output_type, key);

        if (value_changed) {
            output_type.operation = static_cast<int>(OutputOperation::Add);
        }

        break;
    }
    case OutputTarget::DirectDSP: {
        draw_output_direct_dsp(owner, output_type, dsp_index, key);

        if (value_changed) {
            output_type.operation = static_cast<int>(OutputOperation::Add);
        }

        break;
    }
    case OutputTarget::Parameter: {
        value_changed |= draw_output_parameter(owner, output_type, key);

        if (value_changed) {
            output_type.operation = static_cast<int>(OutputOperation::Set);
        }

        break;
    }
    }

    ImGui::Separator();
    ImGui::Text("Operation:");
    prepare_combo(owner, operation_names, "##OutputTypeOperation", output_type.operation, {Target::SPECIAL, key.index, SPECIAL_OPERATION_TYPE});
    ImGui::Text("Variable:");
    prepare_combo(owner, variable_types, "##OutputTypeCombo", output_type.variable_type, {Target::SPECIAL, key.index, SPECIAL_VARIABLE_TYPE});
    ImGui::BeginDisabled(output_type.variable_type == 0);
    draw_int_slider(owner, "Shift", output_type.shift, {Target::SPECIAL, key.index, SPECIAL_SHIFT}, 0, 15);
    ImGui::EndDisabled();
    pop_secondary_style();

    return value_changed;
}

void show_dependency_tooltip(std::vector<std::string> &dependencies) {
    if (ImGui::IsItemHovered() && !dependencies.empty()) {
        std::string tooltip = "Used by: ";
        for (size_t i = 0; i < dependencies.size(); ++i) {
            tooltip += dependencies[i];
            if (i < dependencies.size() - 1) {
                tooltip += ", ";
            }
        }
        ImGui::SetTooltip("%s", tooltip.c_str());
    }
}

void show_commands_pattern_tooltip(const CommandsPattern &pattern, const size_t index) {
    if (ImGui::IsItemHovered()) {
        const std::string &command = pattern.commands[index];
        const std::string &command_value = pattern.values[index];
        if (command.empty()) {
            return;
        }

        const char command_char = command[0];
        const Instruction instruction = command_characters.at(command_char);
        switch (instruction) {
        case Instruction::Empty: {
            return;
        }
        case Instruction::PortamentoUp:
        case Instruction::PortamentoDown: {
            uint8_t channel;
            uint16_t value_portamento;
            pattern.split_portamento_value(command_value, channel, value_portamento);
            const double portamento = pattern.cast_portamento_to_double(value_portamento);
            ImGui::SetTooltip("Portamento %s: channel %d, %.3f semitones", command_char == command_letters.at(Instruction::PortamentoUp) ? "up" : "down", channel, portamento);
            break;
        }
        case Instruction::SetMasterGainer: {
            ImGui::SetTooltip("Master gainer: %.4f", std::stod(command_value));
            break;
        }
        case Instruction::SetBPM: {
            ImGui::SetTooltip("BPM: %d", std::stoi(command_value));
            break;
        }
        case Instruction::SetDivision: {
            ImGui::SetTooltip("Division: %d", std::stoi(command_value));
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
            const bool add = command_char == command_letters.at(Instruction::AddByteValue);
            TargetVariableType target_variable_type;
            Target target;
            uint8_t index;
            uint16_t offset;
            uint32_t value;
            pattern.split_change_value_parts(
                command_value,
                target_variable_type,
                target,
                index,
                offset,
                value
            );

            const std::string target_name = target_names.at(target);
            const std::string variable_type_name = variable_types.at(static_cast<size_t>(target_variable_type));
            ImGui::SetTooltip("%s %s: %s %d, offset %d, value %u", add ? "Add" : "Set", variable_type_name.c_str(), target_name.c_str(), index, offset, value);
            break;
        }
        case Instruction::Count:
        default: {
            throw std::runtime_error("Invalid instruction: " + command);
        }
        }
    }
}

void draw_menu_item(const std::string &name, const std::optional<ShortcutAction> action, const bool checked) {
    if (get_menu_item(name, action, checked)) {
        if (action.has_value()) {
            shortcut_manager.execute_action(action.value());
        }
    }
}

bool get_menu_item(const std::string &name, const std::optional<ShortcutAction> action, const bool checked) {
    if (action.has_value()) {
        return ImGui::MenuItem(name.c_str(), shortcut_manager.get_shortcut_display(action.value()).c_str(), checked);
    }
    return ImGui::MenuItem(name.c_str(), nullptr, checked);
}

GUIState prepare_combo(GUIPanel *owner, const std::vector<std::string> &names, std::string label, int &index, const LinkKey key, const bool error_if_empty, const float margin_right) {
    std::vector<const char *> names_cstr;
    for (const auto &name : names) {
        names_cstr.push_back(name.c_str());
    }

    const float combo_width = ImGui::GetContentRegionAvail().x - margin_right;
    ImGui::SetNextItemWidth(combo_width);

    if (error_if_empty && names.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Border, GUI_ERROR_COLOR);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);
    }

    const int old_value = index;
    const bool value_changed = ImGui::Combo(label.c_str(), &index, names_cstr.data(), names_cstr.size());
    const bool right_clicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);

    if (error_if_empty && names.empty()) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    perform_action(owner, key, index, old_value);
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

std::string get_displayed_row(
    const int row,
    const int absolute_row,
    const RowDisplayStyle row_display
) {
    int displayed_row;
    switch (row_display) {
    case RowDisplayStyle::Absolute: {
        displayed_row = absolute_row;
        break;
    }
    case RowDisplayStyle::Relative: {
        displayed_row = row;
        break;
    }
    case RowDisplayStyle::Page:
    default: {
        displayed_row = absolute_row % gui.page_size;
        break;
    }
    }

    return std::to_string(displayed_row);
}

template <typename T>
void perform_action(
    GUIPanel *owner,
    const LinkKey key,
    T &reference,
    const T old_value
) {
    if (old_value != reference) {
        const std::string label = get_key_name(key);
        const auto value_change = ValueChange<T>(reference, old_value);
        history_manager.add_action(
            std::make_unique<ChangeValueAction<T>>(label, owner, key, value_change)
        );
    }
}

void perform_action_float(
    GUIPanel *owner,
    const LinkKey key,
    float &reference,
    const float old_value,
    const char *format
) {
    std::string old_value_string = convert_double_to_string(old_value, format);
    std::string new_value_string = convert_double_to_string(reference, format);

    if (old_value != reference && old_value_string != new_value_string) {
        const std::string label = get_key_name(key);
        const auto value_change = ValueChange<float>(reference, old_value);
        history_manager.add_action(
            std::make_unique<ChangeValueAction<float>>(label, owner, key, value_change)
        );
    }
}

void perform_action_routing(
    GUIPanel *owner,
    const LinkKey key,
    const InputKey &input_key,
    const LinkKey &new_value,
    const LinkKey &old_value
) {
    if (old_value != new_value) {
        std::string label = key.target == Target::CHANNEL ? "Channel " : "DSP ";
        label += std::to_string(key.index);
        const auto value_change = RoutingChange(input_key, old_value, new_value);
        history_manager.add_action(
            std::make_unique<ChangeRoutingAction>(label, owner, key, value_change)
        );
    }
}

void perform_action_order_sequence(
    GUIPanel *owner,
    const LinkKey key,
    const size_t sequence_index,
    const size_t old_sequence,
    const size_t new_sequence
) {
    if (old_sequence != new_sequence) {
        const std::string label = "Order " + std::to_string(key.index);
        const OrderSequenceChange value_change = {sequence_index, old_sequence, new_sequence};
        history_manager.add_action(
            std::make_unique<ChangeOrderSequenceAction>(label, owner, key, value_change)
        );
    }
}

template <size_t n>
void perform_action_string(
    GUIPanel *owner,
    const LinkKey key,
    char (&buffer)[n],
    const std::string &old_value
) {
    const std::string new_value = buffer;
    if (old_value != new_value) {
        std::string label = get_key_name(key);
        const auto value_change = TextChange<n>(buffer, old_value, new_value);
        history_manager.add_action(
            std::make_unique<ChangeTextAction<n>>(label, owner, key, value_change)
        );
    }
}

void perform_action_note(
    GUIPanel *owner,
    const LinkKey key,
    const PatternRow pattern_row,
    const uint8_t old_note,
    const uint8_t new_note
) {
    if (old_note != new_note) {
        const std::string label = "Sequence " + std::to_string(key.index);
        const NoteChange value_change = {pattern_row, old_note, new_note};
        history_manager.add_action(
            std::make_unique<ChangeNoteAction>(label, owner, key, value_change)
        );
    }
}

void perform_action_add(
    GUIPanel *owner,
    const LinkKey key
) {
    const std::string label = target_names.at(key.target) + " " + std::to_string(key.index);
    AddFunction add = [owner, key]() -> void * {
        void *result;
        switch (key.target) {
        case Target::ENVELOPE: {
            result = song.add_envelope();
            break;
        }
        case Target::SEQUENCE: {
            result = song.add_sequence();
            break;
        }
        case Target::COMMANDS_SEQUENCE: {
            result = song.add_commands_sequence();
            break;
        }
        case Target::ORDER: {
            result = song.add_order();
            break;
        }
        case Target::OSCILLATOR: {
            result = song.add_oscillator();
            break;
        }
        case Target::WAVETABLE: {
            result = song.add_wavetable();
            break;
        }
        case Target::DSP: {
            result = song.add_dsp();
            break;
        }
        case Target::CHANNEL: {
            result = song.add_channel();
            break;
        }
        case Target::COMMANDS_CHANNEL: {
            result = song.add_commands_channel();
            break;
        }
        case Target::DIRECT_OUTPUT:
        case Target::DIRECT_DSP:
        case Target::SPLITTER_OUTPUT:
        case Target::SPLITTER_DSP:
        case Target::SPECIAL:
        case Target::COUNT:
        default: {
            throw std::runtime_error("Invalid target type for add operation: " + std::to_string(static_cast<int>(key.target)));
        }
        }

        owner->update();
        return result;
    };

    RemoveFunction remove = [owner, key](size_t index) -> void {
        switch (key.target) {
        case Target::ENVELOPE: {
            song.remove_envelope(index);
            break;
        }
        case Target::SEQUENCE: {
            song.remove_sequence(index);
            break;
        }
        case Target::COMMANDS_SEQUENCE: {
            song.remove_commands_sequence(index);
            break;
        }
        case Target::ORDER: {
            song.remove_order(index);
            break;
        }
        case Target::OSCILLATOR: {
            song.remove_oscillator(index);
            break;
        }
        case Target::WAVETABLE: {
            song.remove_wavetable(index);
            break;
        }
        case Target::DSP: {
            song.remove_dsp(index);
            break;
        }
        case Target::CHANNEL: {
            song.remove_channel(index);
            break;
        }
        case Target::COMMANDS_CHANNEL: {
            song.remove_commands_channel(index);
            break;
        }
        case Target::DIRECT_OUTPUT:
        case Target::DIRECT_DSP:
        case Target::SPLITTER_OUTPUT:
        case Target::SPLITTER_DSP:
        case Target::SPECIAL:
        case Target::COUNT:
        default: {
            throw std::runtime_error("Invalid target type for remove operation: " + std::to_string(static_cast<int>(key.target)));
        }
        }

        owner->update();
    };
    history_manager.add_action(
        std::make_unique<AddItemAction>(label, owner, key, add, remove)
    );
}

void perform_action_remove(
    GUIPanel *owner,
    const LinkKey key
) {
    const std::string label = target_names.at(key.target) + " " + std::to_string(key.index);
    RemoveFunction remove = [owner, key](size_t index) -> void {
        switch (key.target) {
        case Target::ENVELOPE: {
            song.remove_envelope(index);
            break;
        }
        case Target::SEQUENCE: {
            song.remove_sequence(index);
            break;
        }
        case Target::COMMANDS_SEQUENCE: {
            song.remove_commands_sequence(index);
            break;
        }
        case Target::ORDER: {
            song.remove_order(index);
            break;
        }
        case Target::OSCILLATOR: {
            song.remove_oscillator(index);
            break;
        }
        case Target::WAVETABLE: {
            song.remove_wavetable(index);
            break;
        }
        case Target::DSP: {
            song.remove_dsp(index);
            break;
        }
        case Target::CHANNEL: {
            song.remove_channel(index);
            break;
        }
        case Target::COMMANDS_CHANNEL: {
            song.remove_commands_channel(index);
            break;
        }
        case Target::DIRECT_OUTPUT:
        case Target::DIRECT_DSP:
        case Target::SPLITTER_OUTPUT:
        case Target::SPLITTER_DSP:
        case Target::SPECIAL:
        case Target::COUNT:
        default: {
            throw std::runtime_error("Invalid target type for remove operation: " + std::to_string(static_cast<int>(key.target)));
        }
        }

        owner->update();
    };

    // history_manager.add_action(
    //     std::make_unique<RemoveItemAction>(label, owner, item, key, remove, restore)
    // );
}

template void draw_text<GUI_MAX_STRING_LENGTH>(
    GUIPanel *owner,
    const char *label,
    char (&text)[GUI_MAX_STRING_LENGTH],
    const LinkKey key
);

template void perform_action_string<GUI_MAX_STRING_LENGTH>(
    GUIPanel *owner,
    const LinkKey key,
    char (&buffer)[GUI_MAX_STRING_LENGTH],
    const std::string &old_value
);
