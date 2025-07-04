#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

#define IMGUI_DEFINE_MATH_OPERATORS

#include "../imgui/imgui_internal.h"

#include "../../general.hpp"
#include "../../maps/routing.hpp"
#include "../../song/song.hpp"
#include "../../song/links/link.hpp"
#include "../../song/links/manager.hpp"
#include "../../song/links/target.hpp"
#include "../../song/lock/registry.hpp"
#include "../../structures/channel.hpp"
#include "../../structures/dsp.hpp"
#include "../../utils/math.hpp"
#include "../constants.hpp"
#include "../gui.hpp"
#include "../init.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../utils.hpp"
#include "routings.hpp"

GUIRoutingsPanel::GUIRoutingsPanel(const bool visible, const bool windowed)
    : GUIPanel("Routings", visible, windowed) {
}

GUIElement GUIRoutingsPanel::get_element() const {
    return GUIElement::Routings;
}

void GUIRoutingsPanel::draw() {
    draw_reset_button();
    draw_nodes();
    draw_all_links();
}

bool GUIRoutingsPanel::is_disabled() const {
    return gui.is_playing();
}

void GUIRoutingsPanel::from() {
    from_nodes();
    from_links();
}

void GUIRoutingsPanel::to() const {
    if (!save &&
        (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ||
         gui.is_playing())) {
        return;
    }

    to_nodes();
    to_links();
}

void GUIRoutingsPanel::to_nodes() const {
    for (const auto &node : nodes) {
        if (node.key.has_value()) {
            const InputKey &input_key = node.key.value();
            if (input_key.first == ItemType::CHANNEL) {
                const size_t channel_index = input_key.second;
                Channel *channel = channels[channel_index];
                set_flag(channel->flag, FLAG_BYPASS, node.bypass);
            } else if (input_key.first == ItemType::DSP) {
                const size_t dsp_index = input_key.second;
                DSP *dsp = static_cast<DSP *>(dsps[dsp_index]);
                set_flag(dsp->flag, FLAG_BYPASS, node.bypass);
            }
        }
    }
}

void GUIRoutingsPanel::to_links() const {
    for (const auto &[source_key, target_key] : nodes_links) {
        const ItemType type = source_key.first;
        const size_t id = source_key.second;

        const size_t type_index = static_cast<size_t>(type);
        if (id >= links[type_index].size()) {
            const std::string &name = item_types_names.at(type);
            std::cerr << "Error: " << name << " " << id
                      << " is not available" << std::endl;
            continue;
        }

        Link &link = links[type_index][id];
        bool locked = is_node_locked(source_key);

        if (!locked && is_linking_possible(source_key, target_key)) {
            link.target = target_key.target;
            link.index = target_key.index;
            link.offset = target_key.offset;
            link_manager.set_link(link, link.item, id);
        }
    }

    link_manager.save_targets();
}

void GUIRoutingsPanel::from_links() {
    nodes_links.clear();
    for (const ItemType type : {ItemType::CHANNEL, ItemType::DSP}) {
        for (const auto &link : links[static_cast<size_t>(type)]) {
            nodes_links[{type, link.id}] = link.key;
        }
    }
}

void GUIRoutingsPanel::from_nodes() {
    if (ImGui::GetCurrentContext() == nullptr) {
        return;
    }

    std::unordered_map<NodeIdentifier, size_t> existing_node_indices;
    for (size_t i = 0; i < nodes.size(); ++i) {
        existing_node_indices[nodes[i].identifier] = i;
    }

    std::vector<RoutingNode> next_nodes;
    next_nodes.reserve(channels.size() + dsps.size() + song.get_output_channels());

    std::map<float, float> column_next_y;
    const float channel_x = GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float output_x = dsp_x + GUI_ROUTING_NODE_WIDTH + GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float vertical_padding = GUI_ROUTING_NODE_VERTICAL_PADDING;
    const float initial_y_offset = GUI_ROUTING_NODE_VERTICAL_PADDING;
    column_next_y[channel_x] = initial_y_offset;
    column_next_y[dsp_x] = initial_y_offset;
    column_next_y[output_x] = initial_y_offset;

    for (const auto &node : nodes) {
        if (column_next_y.count(node.position.x) != 0u) {
            float node_height = node.size.y > 0 ? node.size.y : (node.lines * ImGui::GetTextLineHeight());
            column_next_y[node.position.x] = std::max(column_next_y[node.position.x], node.position.y + node_height + vertical_padding);
        }
    }

    column_next_y[channel_x] = std::max(column_next_y[channel_x], GUI_ROUTING_NODE_VERTICAL_PADDING);
    column_next_y[dsp_x] = std::max(column_next_y[dsp_x], 0.0f);
    column_next_y[output_x] = std::max(column_next_y[output_x], GUI_ROUTING_NODE_VERTICAL_PADDING);

    for (size_t i = 0; i < channels.size(); ++i) {
        NodeIdentifier current_id = {Target::CHANNEL, i};
        auto it = existing_node_indices.find(current_id);

        if (it != existing_node_indices.end()) {
            RoutingNode &node = nodes[it->second];
            update_channel_node(i, node);
            next_nodes.push_back(node);
            existing_node_indices.erase(it);
        } else {
            add_channel_node(i, next_nodes, column_next_y);
        }
    }

    for (size_t i = 0; i < dsps.size(); ++i) {
        NodeIdentifier current_id = {Target::DSP, i};
        auto it = existing_node_indices.find(current_id);

        if (it != existing_node_indices.end()) {
            RoutingNode &node = nodes[it->second];
            update_dsp_node(i, node);
            next_nodes.push_back(node);
            existing_node_indices.erase(it);
        } else {
            add_dsp_node(i, next_nodes, column_next_y);
        }
    }

    const size_t output_channels_count = song.get_output_channels();
    for (size_t i = 0; i < output_channels_count; ++i) {
        NodeIdentifier current_id = {Target::DIRECT_OUTPUT, i};
        auto it = existing_node_indices.find(current_id);

        if (it != existing_node_indices.end()) {
            RoutingNode &node = nodes[it->second];
            update_output_node(i, node);
            next_nodes.push_back(node);
            existing_node_indices.erase(it);
        } else {
            add_output_node(i, next_nodes, column_next_y);
        }
    }

    nodes = std::move(next_nodes);
}

void GUIRoutingsPanel::update_channel_node(size_t index, RoutingNode &channel_node) {
    channel_node.name = channel_names[index];
    channel_node.parameters.clear();
    channel_node.lines = 1;

    const Channel *channel = channels[index];
    const Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][index];
    const auto &channel_routing = routing_variables.at(Target::CHANNEL);
    const auto filtered_items = channel_routing.filter_items(-1);
    const auto &labels = std::get<1>(filtered_items);
    const auto &offsets = std::get<2>(filtered_items);
    channel_node.bypass = channel->flag & FLAG_BYPASS;
    channel_solo |= channel_node.solo;

    for (size_t j = 0; j < labels.size(); ++j) {
        if (offsets[j] >= CHANNEL_SPLITTER && offsets[j] < CHANNEL_SPLITTER + MAX_OUTPUT_CHANNELS) {
            const int splitter_id = offsets[j] - CHANNEL_SPLITTER;
            if (get_splitter_bounds(splitter_id, index, link)) {
                continue;
            }
        }

        const OutputKey key = {Target::CHANNEL, static_cast<int>(index), offsets[j]};
        channel_node.parameters.emplace_back(key, labels[j]);
        channel_node.lines += 1;
    }
}

void GUIRoutingsPanel::add_channel_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float vertical_padding = GUI_ROUTING_NODE_VERTICAL_PADDING;

    RoutingNode channel_node;
    NodeIdentifier current_id = {Target::CHANNEL, index};
    channel_node.identifier = current_id;
    channel_node.key = {ItemType::CHANNEL, static_cast<int>(index)};
    update_channel_node(index, channel_node);

    const float estimated_height = channel_node.lines * ImGui::GetTextLineHeight();
    channel_node.position = {channel_x, column_next_y[channel_x]};
    column_next_y[channel_x] += estimated_height + vertical_padding;

    next_nodes.push_back(channel_node);
}

void GUIRoutingsPanel::update_dsp_node(size_t index, RoutingNode &dsp_node) {
    dsp_node.name = dsp_names[index];
    dsp_node.parameters.clear();
    dsp_node.lines = 1;

    const DSP *dsp = static_cast<DSP *>(dsps[index]);
    const Link &link = links[static_cast<size_t>(ItemType::DSP)][index];
    const auto &dsp_routing = routing_variables.at(Target::DSP);
    const auto filtered_items = dsp_routing.filter_items(dsp->effect_index);
    const auto &labels = std::get<1>(filtered_items);
    const auto &offsets = std::get<2>(filtered_items);
    dsp_node.bypass = dsp->flag & FLAG_BYPASS;
    dsp_solo |= dsp_node.solo;

    for (size_t j = 0; j < labels.size(); ++j) {
        if (offsets[j] >= DSP_SPLITTER && offsets[j] < DSP_SPLITTER + MAX_OUTPUT_CHANNELS) {
            const int splitter_id = offsets[j] - DSP_SPLITTER;
            if (get_splitter_bounds(splitter_id, index, link)) {
                continue;
            }
        }

        const OutputKey key = {Target::DSP, static_cast<int>(index), offsets[j]};
        dsp_node.parameters.emplace_back(key, labels[j]);
        dsp_node.lines += 1;
    }
}

void GUIRoutingsPanel::add_dsp_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float vertical_padding = GUI_ROUTING_NODE_VERTICAL_PADDING;

    RoutingNode dsp_node;
    NodeIdentifier current_id = {Target::DSP, index};
    dsp_node.identifier = current_id;
    dsp_node.key = {ItemType::DSP, static_cast<int>(index)};
    update_dsp_node(index, dsp_node);

    const float estimated_height = dsp_node.lines * ImGui::GetTextLineHeight();
    dsp_node.position = {dsp_x, column_next_y[dsp_x]};
    column_next_y[dsp_x] += estimated_height + vertical_padding;

    next_nodes.push_back(dsp_node);
}

void GUIRoutingsPanel::update_output_node(size_t index, RoutingNode &output_node) {
    output_node.name = "Output " + std::to_string(index);
    output_node.parameters.clear();
    output_node.lines = 1;
}

void GUIRoutingsPanel::add_output_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float output_x = dsp_x + GUI_ROUTING_NODE_WIDTH + GUI_ROUTING_NODE_HORIZONTAL_PADDING;
    const float vertical_padding = GUI_ROUTING_NODE_VERTICAL_PADDING;

    RoutingNode output_node;
    NodeIdentifier current_id = {Target::DIRECT_OUTPUT, index};
    output_node.identifier = current_id;
    update_output_node(index, output_node);

    const float estimated_height = output_node.lines * ImGui::GetTextLineHeight();
    output_node.position = {output_x, column_next_y[output_x]};
    column_next_y[output_x] += estimated_height + vertical_padding;

    next_nodes.push_back(output_node);
}

void GUIRoutingsPanel::draw_reset_button() {
    if (ImGui::Button("Reset view", ImVec2(100.0f, 0))) {
        clear_nodes();
        from_nodes();
        from_links();
    }

    ImGui::Separator();
}

void GUIRoutingsPanel::draw_nodes() {
    if (ImGui::IsWindowCollapsed()) {
        reset_dragging();
        reset_linking();
        return;
    }

    input_pins.clear();
    output_pins.clear();

    ImVec2 content_size = calculate_content_size();

    ImGui::SetNextWindowContentSize(content_size);
    ImGui::BeginChild("RoutingCanvas", ImVec2(0, 0), 0, ImGuiWindowFlags_HorizontalScrollbar);

    const ImVec2 canvas_origin = ImGui::GetCursorScreenPos();
    const ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    const ImVec2 canvas_p1 = ImVec2(canvas_origin.x + canvas_sz.x, canvas_origin.y + canvas_sz.y);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRect(canvas_origin, canvas_p1, true);

    handle_node_dragging(canvas_origin);

    for (RoutingNode &node : nodes) {
        const ImVec2 node_draw_pos = canvas_origin + node.position;
        draw_node(node, node_draw_pos);
    }

    draw_list->PopClipRect();
    ImGui::EndChild();
}

void GUIRoutingsPanel::draw_link(const InputKey &source_key, const OutputKey &target_key, uint8_t alpha) {
    const float line_thickness = GUI_ROUTING_LINK_THICKNESS;
    const ImU32 audio_color = IM_COL32(0, 200, 0, alpha);
    const ImU32 parameter_color = IM_COL32(200, 150, 0, alpha);
    const ImU32 dragging_color = IM_COL32(150, 150, 150, 255);

    /* don't show invalid DSP links */
    if (!is_linking_possible(source_key, target_key)) {
        return;
    }

    const bool dragging = link_dragging_source_key.has_value() && link_dragging_source_key.value() == source_key;
    auto source_pin_it = output_pins.find(source_key);
    auto dest_pin_it = input_pins.find(target_key);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    if (source_pin_it != output_pins.end() && dest_pin_it != input_pins.end()) {
        ImVec2 p1;
        ImVec2 p4;
        ImU32 color;
        if (dragging) {
            const ImVec2 mouse_pos = ImGui::GetMousePos();
            p1 = source_pin_it->second;
            p4 = mouse_pos;
            color = dragging_color;
        } else {
            p1 = source_pin_it->second;
            p4 = dest_pin_it->second;
            const bool is_audio_link = target_key.target == Target::DIRECT_OUTPUT || target_key.target == Target::DIRECT_DSP;
            color = is_audio_link ? audio_color : parameter_color;
        }

        const float horizontal_distance = std::abs(p4.x - p1.x);
        const float control_offset_x = std::max(30.0f, horizontal_distance * 0.4f);

        const ImVec2 p2 = ImVec2(p1.x + control_offset_x, p1.y);
        const ImVec2 p3 = ImVec2(p4.x - control_offset_x, p4.y);

        draw_list->AddBezierCubic(p1, p2, p3, p4, color, line_thickness, GUI_ROUTING_CURVE_SEGMENTS);
    }
}

Splitter GUIRoutingsPanel::get_splitter_from_input_key(const InputKey &source) {
    Splitter splitter;
    switch (source.first) {
    case ItemType::CHANNEL: {
        std::copy(
            std::begin(channels[source.second]->splitter),
            std::end(channels[source.second]->splitter),
            splitter.begin()
        );
        break;
    }
    case ItemType::DSP: {
        DSP *dsp = reinterpret_cast<DSP *>(dsps[source.second]);
        std::copy(
            std::begin(dsp->splitter),
            std::end(dsp->splitter),
            splitter.begin()
        );
        break;
    }
    case ItemType::COMMANDS:
    case ItemType::COUNT:
    default:
        throw std::runtime_error("Invalid source type for routing link: " + std::to_string(static_cast<int>(source.first)));
    }

    return splitter;
}

void GUIRoutingsPanel::draw_all_links() {
    for (const auto &[source, target] : nodes_links) {
        const Splitter splitter = get_splitter_from_input_key(source);
        if (target.target == Target::SPLITTER_OUTPUT) {
            const size_t output_channels = song.get_output_channels();
            for (size_t i = 0; i < output_channels; ++i) {
                const uint16_t offset = sizeof(_Float32) * i;
                const OutputKey target_key = {Target::DIRECT_OUTPUT, static_cast<int>(i), offset};
                draw_link(source, target_key, splitter[i]);
            }
        } else if (target.target == Target::SPLITTER_DSP) {
            int start = target.offset / sizeof(_Float32);
            int end = clamp(static_cast<int>(dsps.size()) - start, 0, MAX_OUTPUT_CHANNELS);
            for (int i = 0; i < end; ++i) {
                const size_t j = start + i;
                const uint16_t offset = sizeof(_Float32) * j;
                const OutputKey target_key = {Target::DIRECT_DSP, static_cast<int>(j), offset};
                draw_link(source, target_key, splitter[i]);
            }
        } else {
            draw_link(source, target);
        }
    }

    const bool dragging = link_dragging_source_key.has_value();
    if (dragging) {
        draw_link(
            link_dragging_source_key.value(),
            OutputKey{Target::DIRECT_OUTPUT, 0, 0},
            150
        );
    }
}

void GUIRoutingsPanel::draw_node(RoutingNode &routing_node, const ImVec2 node_rect_min) {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    std::string id_prefix;
    switch (routing_node.identifier.type) {
    case Target::CHANNEL:
        id_prefix = "ChannelNode_";
        break;
    case Target::COMMANDS_CHANNEL:
        id_prefix = "CommandsChannelNode_";
        break;
    case Target::DSP:
    case Target::DIRECT_DSP:
        id_prefix = "DSPNode_";
        break;
    case Target::SPLITTER_OUTPUT:
        id_prefix = "SplitterOutputNode_";
        break;
    case Target::SPLITTER_DSP:
        id_prefix = "SplitterDSPNode_";
        break;
    case Target::DIRECT_OUTPUT:
        id_prefix = "OutputNode_";
        break;
    case Target::ENVELOPE:
        id_prefix = "EnvelopeNode_";
        break;
    case Target::SEQUENCE:
        id_prefix = "SequenceNode_";
        break;
    case Target::COMMANDS_SEQUENCE:
        id_prefix = "CommandsSequenceNode_";
        break;
    case Target::ORDER:
        id_prefix = "OrderNode_";
        break;
    case Target::OSCILLATOR:
        id_prefix = "OscillatorNode_";
        break;
    case Target::WAVETABLE:
        id_prefix = "WavetableNode_";
        break;
    case Target::SPECIAL:
        id_prefix = "SpecialNode_";
        break;
    case Target::COUNT:
    default:
        throw std::runtime_error("Invalid target type: " + std::to_string(static_cast<int>(routing_node.identifier.type)));
    }
    const std::string id_str = id_prefix + std::to_string(routing_node.identifier.id);
    ImGui::PushID(id_str.c_str());

    const ImGuiStyle &style = ImGui::GetStyle();
    const float node_padding_x = style.WindowPadding.x;
    const float node_padding_y = style.WindowPadding.y / 2.0f;
    const float line_height = ImGui::GetTextLineHeight();

    const bool locked = is_node_locked(routing_node);
    uint8_t node_alpha = get_bypass_state(routing_node) ? 128 : 255;
    if (locked) {
        node_alpha = static_cast<uint8_t>(static_cast<float>(node_alpha) * 0.6f);
    }

    const float node_content_height = routing_node.lines * line_height + (routing_node.lines > 1 ? (routing_node.lines - 1) * style.ItemSpacing.y : 0);
    const float node_actual_height = node_content_height + node_padding_y * 2.0f;

    const float name_width = ImGui::CalcTextSize(routing_node.name.c_str()).x;
    float max_param_width = 0.0f;
    if (!routing_node.parameters.empty()) {
        for (const auto &[parameter_key, parameter_label] : routing_node.parameters) {
            max_param_width = std::max(max_param_width, ImGui::CalcTextSize(parameter_label.c_str()).x);
        }
    }
    const float content_width = std::max(name_width, max_param_width);
    const float node_actual_width = std::max(GUI_ROUTING_NODE_WIDTH, content_width + node_padding_x * 2.0f);

    const ImVec2 node_rect_max = {node_rect_min.x + node_actual_width, node_rect_min.y + node_actual_height};
    routing_node.size = {node_actual_width, node_actual_height};

    const bool is_hovered = ImGui::IsMouseHoveringRect(node_rect_min, node_rect_max);
    if (is_hovered && locked) {
        ImGui::SetTooltip("This node is locked.");
    }

    if (
        ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
        !gui.is_playing() &&
        is_hovered &&
        routing_node.key.has_value()
    ) {
        if (ImGui::GetIO().KeyShift) {
            const bool solo = !routing_node.solo;
            const ItemType type = routing_node.key.has_value() ? routing_node.key.value().first : ItemType::COUNT;
            for (RoutingNode &node : nodes) {
                if (node.key.has_value() && node.key.value().first == type) {
                    node.bypass = solo;
                    node.solo = false;
                }
            }

            routing_node.solo = solo;
            routing_node.bypass = false;
        } else {
            routing_node.bypass = !routing_node.bypass;
        }

        save = true;
    }

    if (
        ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        !dragging_node_id.has_value() &&
        is_hovered
    ) {
        dragging_node_id = routing_node.identifier;
        drag_node_offset = ImGui::GetMousePos() - node_rect_min;
        const Target target = routing_node.identifier.type;
        if (target == Target::CHANNEL) {
            gui.set_index(GUIElement::Channels, routing_node.identifier.id);
        } else if (target == Target::DSP) {
            gui.set_index(GUIElement::DSPs, routing_node.identifier.id);
        }
    }

    const uint8_t bg_alpha = routing_node.bypass ? 100 : 200;
    ImU32 node_bg_color = is_hovered ? IM_COL32(70, 70, 70, bg_alpha) : IM_COL32(50, 50, 50, bg_alpha);
    draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
    draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, node_alpha), 4.0f);

    const uint8_t alpha = link_dragging_source_key.has_value() ? 100 : 255;
    const ImU32 pin_color_main = IM_COL32(255, 255, 255, alpha);
    const ImU32 pin_color_parameter = IM_COL32(150, 150, 150, alpha);
    const ImU32 pin_color_available = IM_COL32(250, 250, 150, 255);
    const float pin_offset_y = line_height * 0.5f;

    ImVec2 current_text_pos = ImVec2(node_rect_min.x + node_padding_x, node_rect_min.y + node_padding_y);
    float pin_y = current_text_pos.y + pin_offset_y;

    if (routing_node.identifier.type != Target::DIRECT_OUTPUT) {
        const ImVec2 pin_position = ImVec2(node_rect_max.x + GUI_ROUTING_PIN_RADIUS, pin_y);
        draw_list->AddCircleFilled(pin_position, GUI_ROUTING_PIN_RADIUS, pin_color_main);
        const InputKey key = routing_node.key.value();
        output_pins[key] = pin_position;

        if (!locked) {
            set_source_key(pin_position, key);
        }
    }

    if (routing_node.identifier.type != Target::CHANNEL) {
        const int index = routing_node.identifier.id;
        const uint16_t offset = sizeof(_Float32) * routing_node.identifier.id;
        const Target target = routing_node.identifier.type == Target::DSP ? Target::DIRECT_DSP : Target::DIRECT_OUTPUT;
        const OutputKey key = {target, index, offset};

        const bool linking_possible = link_dragging_source_key.has_value() && is_linking_possible(link_dragging_source_key.value(), key);
        const ImU32 color = linking_possible ? pin_color_available : pin_color_main;
        const ImVec2 pin_position = ImVec2(node_rect_min.x - GUI_ROUTING_PIN_RADIUS, pin_y);
        draw_list->AddCircleFilled(pin_position, GUI_ROUTING_PIN_RADIUS, color);

        if (!locked) {
            input_pins[key] = pin_position;
            set_target_key(pin_position, key);
        }
    }

    ImVec4 text_color_vector = routing_node.bypass ? ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled) : ImGui::GetStyleColorVec4(ImGuiCol_Text);
    ImU32 text_color = ImGui::ColorConvertFloat4ToU32(text_color_vector);
    draw_list->AddText(current_text_pos, text_color, routing_node.name.c_str());
    current_text_pos.y += line_height + style.ItemSpacing.y;

    for (const auto &parameter : routing_node.parameters) {
        const auto &[parameter_key, parameter_label] = parameter;
        pin_y = current_text_pos.y + pin_offset_y;
        const ImVec2 pin_position = ImVec2(node_rect_min.x - GUI_ROUTING_PIN_RADIUS, pin_y);
        const ImU32 color = link_dragging_source_key.has_value() ? pin_color_available : pin_color_parameter;
        draw_list->AddCircleFilled(pin_position, GUI_ROUTING_PIN_RADIUS, color);
        draw_list->AddText(current_text_pos, routing_node.bypass ? text_color : color, parameter_label.c_str());
        current_text_pos.y += line_height + style.ItemSpacing.y;
        input_pins[parameter_key] = pin_position;
        set_target_key(pin_position, parameter_key);
    }

    ImGui::PopID();
}

RoutingNode *GUIRoutingsPanel::handle_node_dragging(const ImVec2 &canvas_origin) {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || gui.is_playing()) {
        return nullptr;
    }

    RoutingNode *current_dragging_node_ptr = nullptr;

    if (dragging_node_id.has_value()) {
        for (RoutingNode &node : nodes) {
            if (node.identifier == dragging_node_id.value()) {
                current_dragging_node_ptr = &node;
                break;
            }
        }

        if (current_dragging_node_ptr != nullptr) {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 current_mouse_pos = ImGui::GetMousePos();
                current_dragging_node_ptr->position = current_mouse_pos - canvas_origin - drag_node_offset;
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                dragging_node_id.reset();
                current_dragging_node_ptr = nullptr;
            }
        } else {
            dragging_node_id.reset();
        }
    }
    return current_dragging_node_ptr;
}

void GUIRoutingsPanel::set_source_key(const ImVec2 pin_position, const InputKey &source_key) {
    if (gui.is_playing()) {
        return;
    }

    const ImVec2 radius = ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS);
    const bool is_pin_hovered = ImGui::IsMouseHoveringRect(pin_position - radius, pin_position + radius);
    if (is_pin_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        link_dragging_source_key = source_key;
    } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        link_dragging_source_key = std::nullopt;
    }
}

void GUIRoutingsPanel::set_target_key(const ImVec2 pin_position, const OutputKey &target_key) {
    if (gui.is_playing()) {
        return;
    }

    const ImVec2 radius = ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS);
    const bool is_pin_hovered = ImGui::IsMouseHoveringRect(pin_position - radius, pin_position + radius);
    if (link_dragging_source_key.has_value() && is_pin_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        const InputKey source_key = link_dragging_source_key.value();
        const OutputKey old_value = nodes_links[source_key];
        set_link(source_key, target_key);
        link_dragging_source_key = std::nullopt;
        const LinkKey link_key = from_input_key(source_key);
        perform_action_routing(this, link_key, source_key, target_key, old_value);
    }
}

void GUIRoutingsPanel::reset_dragging() {
    dragging_node_id = std::nullopt;
}

void GUIRoutingsPanel::reset_linking() {
    link_dragging_source_key = std::nullopt;
}

bool GUIRoutingsPanel::is_linking_possible(const InputKey &source_key, const OutputKey &target_key) {
    const ItemType type = source_key.first;
    const size_t id = source_key.second;
    const size_t item = target_key.index;
    const Target target = target_key.target;
    return (item > id || type != ItemType::DSP || (target != Target::DIRECT_DSP && target != Target::SPLITTER_DSP));
}

bool GUIRoutingsPanel::get_bypass_state(const RoutingNode &node) const {
    if (!node.key.has_value()) {
        return false;
    }

    const ItemType type = node.key->first;
    const bool bypass = node.bypass;
    const bool solo = node.solo;

    return get_bypass_state(type, bypass, solo);
}

bool GUIRoutingsPanel::get_bypass_state(const ItemType type, const bool bypass, const bool solo) const {
    if (type == ItemType::CHANNEL) {
        return channel_solo ? bypass & solo : bypass;
    }

    if (type == ItemType::DSP) {
        return dsp_solo ? bypass & solo : bypass;
    }

    return bypass;
}

bool GUIRoutingsPanel::get_splitter_bounds(const size_t j, size_t index, const Link &link) {
    if (!is_target_splitter(link.target)) {
        return true;
    }

    if (link.target == Target::SPLITTER_OUTPUT) {
        if (j >= song.get_output_channels()) {
            return true;
        }
    } else if (link.target == Target::SPLITTER_DSP) {
        const size_t offset = link.type == ItemType::DSP ? std::max(index + 1, static_cast<size_t>(link.index)) : link.index;
        if (j + offset >= dsps.size()) {
            return true;
        }
    }

    return false;
}

void GUIRoutingsPanel::clear_nodes() {
    nodes.clear();
    input_pins.clear();
    output_pins.clear();
    nodes_links.clear();
    dragging_node_id.reset();
    link_dragging_source_key.reset();
}

void GUIRoutingsPanel::set_link(const InputKey &source_key, const OutputKey &target_key) {
    nodes_links[source_key] = target_key;
}

std::vector<std::pair<NodeIdentifier, ImVec2>> GUIRoutingsPanel::get_nodes_positions() const {
    std::vector<std::pair<NodeIdentifier, ImVec2>> nodes_positions;
    nodes_positions.reserve(nodes.size());

    for (const RoutingNode &node : nodes) {
        nodes_positions.emplace_back(node.identifier, node.position);
    }

    return nodes_positions;
}

void GUIRoutingsPanel::set_nodes_positions(const std::vector<std::pair<NodeIdentifier, ImVec2>> &nodes_positions) {
    for (const auto &[identifier, position] : nodes_positions) {
        for (RoutingNode &node : nodes) {
            if (node.identifier == identifier) {
                node.position = position;
                break;
            }
        }
    }
}

ImVec2 GUIRoutingsPanel::calculate_content_size() const {
    ImVec2 min_pos(FLT_MAX, FLT_MAX);
    ImVec2 max_pos(-FLT_MAX, -FLT_MAX);

    for (const auto &node : nodes) {
        min_pos.x = std::min(min_pos.x, node.position.x);
        min_pos.y = std::min(min_pos.y, node.position.y);
        max_pos.x = std::max(max_pos.x, node.position.x + node.size.x);
        max_pos.y = std::max(max_pos.y, node.position.y + node.size.y);
    }

    min_pos.x = std::min(min_pos.x, 0.0f);
    min_pos.y = std::min(min_pos.y, 0.0f);
    max_pos.x = std::max(max_pos.x + 100.0f, ImGui::GetContentRegionAvail().x);
    max_pos.y = std::max(max_pos.y + 100.0f, ImGui::GetContentRegionAvail().y);

    const ImVec2 content_size(std::max(max_pos.x - min_pos.x, ImGui::GetContentRegionAvail().x), std::max(max_pos.y - min_pos.y, ImGui::GetContentRegionAvail().y));
    return content_size;
}

bool GUIRoutingsPanel::is_node_locked(const RoutingNode &node) {
    if (!node.key.has_value()) {
        return false;
    }

    return is_node_locked(node.key.value());
}

bool GUIRoutingsPanel::is_node_locked(const InputKey input_key) {
    const ItemType type = input_key.first;
    const size_t id = input_key.second;

    if (type == ItemType::CHANNEL) {
        return lock_registry.is_locked(Target::CHANNEL, id);
    }

    if (type == ItemType::DSP) {
        return lock_registry.is_locked(Target::DSP, id);
    }

    return false;
}
