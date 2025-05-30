#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#include "../init.hpp"

#include "../../general.hpp"
#include "../../maps/routing.hpp"
#include "../../song/links/link.hpp"
#include "../../song/song.hpp"
#include "../../structures/channel.hpp"
#include "../../structures/dsp.hpp"
#include "../constants.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "routing.hpp"

GUIRoutingPanel::GUIRoutingPanel(const bool visible)
    : GUIPanel(visible) {
}

void GUIRoutingPanel::update() {
}

void GUIRoutingPanel::draw() {
    ImGui::Begin("Routings");
    ImGui::BeginChild("RoutingCanvas", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    from();
    draw_nodes();
    draw_all_links();
    check_keyboard_input();
    to();

    ImGui::EndChild();
    ImGui::End();
}

void GUIRoutingPanel::from() {
    collect_nodes();
    collect_links();
}

void GUIRoutingPanel::to() const {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || gui.is_playing()) {
        return;
    }

    for (const auto &[source_key, target_key] : nodes_links) {
        const ItemType type = source_key.first;
        const size_t id = source_key.second;
        Link &link = links[static_cast<size_t>(type)][id];
        if (is_linking_possible(source_key, target_key)) {
            link.target = target_key.target;
            link.index = target_key.index;
            link.offset = target_key.offset;
            link_manager.set_link(link, link.item, id);
        }
    }

    link_manager.save_targets();
}

void GUIRoutingPanel::collect_links() {
    nodes_links.clear();
    for (const ItemType type : {ItemType::CHANNEL, ItemType::DSP}) {
        for (const auto &link : links[static_cast<size_t>(type)]) {
            nodes_links[{type, link.id}] = link.key;
        }
    }
}

void GUIRoutingPanel::collect_nodes() {
    std::unordered_map<NodeIdentifier, size_t> existing_node_indices;
    for (size_t i = 0; i < nodes.size(); ++i) {
        existing_node_indices[nodes[i].identifier] = i;
    }

    std::vector<RoutingNode> next_nodes;
    next_nodes.reserve(channels.size() + dsps.size() + song.get_output_channels());

    std::map<float, float> column_next_y;
    const float channel_x = 50.0f;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float output_x = dsp_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float vertical_padding = 20.0f;
    const float initial_y_offset = 20.0f;
    column_next_y[channel_x] = initial_y_offset;
    column_next_y[dsp_x] = initial_y_offset;
    column_next_y[output_x] = initial_y_offset;

    for (const auto &node : nodes) {
        if (column_next_y.count(node.position.x)) {
            float node_height = node.size.y > 0 ? node.size.y : (node.lines * ImGui::GetTextLineHeight());
            column_next_y[node.position.x] = std::max(column_next_y[node.position.x], node.position.y + node_height + vertical_padding);
        }
    }

    column_next_y[channel_x] = std::max(column_next_y[channel_x], 25.0f);
    column_next_y[dsp_x] = std::max(column_next_y[dsp_x], 0.0f);
    column_next_y[output_x] = std::max(column_next_y[output_x], 25.0f);

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

void GUIRoutingPanel::update_channel_node(size_t index, RoutingNode &channel_node) {
    channel_node.name = channel_names[index];
    channel_node.parameters.clear();
    channel_node.lines = 1;

    const Channel *channel = channels[index];
    const Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][index];
    const auto &channel_routing = routing_variables.at(Target::CHANNEL);
    channel_node.bypass = channel->flag & FLAG_BYPASS;

    for (size_t j = 0; j < channel_routing.labels.size(); ++j) {
        if (channel_routing.offsets[j] >= CHANNEL_SPLITTER && channel_routing.offsets[j] < CHANNEL_SPLITTER + 4) {
            const int splitter_id = channel_routing.offsets[j] - CHANNEL_SPLITTER;
            if (get_splitter_bounds(splitter_id, index, link)) {
                continue;
            }
        }

        const OutputKey key = {Target::CHANNEL, static_cast<int>(index), channel_routing.offsets[j]};
        channel_node.parameters.push_back({key, channel_routing.labels[j]});
        channel_node.lines += 1;
    }
}

void GUIRoutingPanel::add_channel_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = 50.0f;
    const float vertical_padding = 20.0f;

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

void GUIRoutingPanel::update_dsp_node(size_t index, RoutingNode &dsp_node) {
    dsp_node.name = dsp_names[index];
    dsp_node.parameters.clear();
    dsp_node.lines = 1;

    const DSP *dsp = static_cast<DSP *>(dsps[index]);
    const Link &link = links[static_cast<size_t>(ItemType::DSP)][index];
    const auto &dsp_routing = routing_variables.at(Target::DSP);
    const auto filtered_items = dsp_routing.filter_items(dsp->effect_index);
    const auto labels = std::get<1>(filtered_items);
    const auto offsets = std::get<2>(filtered_items);
    dsp_node.bypass = dsp->flag & FLAG_BYPASS;

    for (size_t j = 0; j < labels.size(); ++j) {
        if (offsets[j] >= DSP_SPLITTER && offsets[j] < DSP_SPLITTER + 4) {
            const int splitter_id = offsets[j] - DSP_SPLITTER;
            if (get_splitter_bounds(splitter_id, index, link)) {
                continue;
            }
        }

        const OutputKey key = {Target::DSP, static_cast<int>(index), offsets[j]};
        dsp_node.parameters.push_back({key, labels[j]});
        dsp_node.lines += 1;
    }
}

void GUIRoutingPanel::add_dsp_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = 50.0f;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float vertical_padding = 20.0f;

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

void GUIRoutingPanel::update_output_node(size_t index, RoutingNode &output_node) {
    output_node.name = "Output " + std::to_string(index);
    output_node.parameters.clear();
    output_node.lines = 1;
}

void GUIRoutingPanel::add_output_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = 50.0f;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float output_x = dsp_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float vertical_padding = 20.0f;

    RoutingNode output_node;
    NodeIdentifier current_id = {Target::DIRECT_OUTPUT, index};
    output_node.identifier = current_id;
    update_output_node(index, output_node);

    const float estimated_height = output_node.lines * ImGui::GetTextLineHeight();
    output_node.position = {output_x, column_next_y[output_x]};
    column_next_y[output_x] += estimated_height + vertical_padding;

    next_nodes.push_back(output_node);
}

void GUIRoutingPanel::draw_nodes() {
    input_pins.clear();
    output_pins.clear();

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
}

void GUIRoutingPanel::draw_link(const InputKey source, const OutputKey target, uint8_t alpha) {
    const float line_thickness = 1.5f;
    const ImU32 audio_color = IM_COL32(0, 200, 0, alpha);
    const ImU32 parameter_color = IM_COL32(200, 150, 0, alpha);
    const ImU32 dragging_color = IM_COL32(150, 150, 150, 255);

    /* don't show invalid DSP links */
    if (!is_linking_possible(source, target)) {
        return;
    }

    const bool dragging = link_dragging_source_key.has_value() && link_dragging_source_key.value() == source;
    auto source_pin_it = output_pins.find(source);
    auto dest_pin_it = input_pins.find(target);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    if (source_pin_it != output_pins.end() && dest_pin_it != input_pins.end()) {
        ImVec2 p1, p4;
        ImU32 color;
        if (dragging) {
            const ImVec2 mouse_pos = ImGui::GetMousePos();
            p1 = source_pin_it->second;
            p4 = mouse_pos;
            color = dragging_color;
        } else {
            p1 = source_pin_it->second;
            p4 = dest_pin_it->second;
            const bool is_audio_link = target.target == Target::DIRECT_OUTPUT || target.target == Target::DIRECT_DSP;
            color = is_audio_link ? audio_color : parameter_color;
        }

        const float horizontal_distance = std::abs(p4.x - p1.x);
        const float control_offset_x = std::max(30.0f, horizontal_distance * 0.4f);

        const ImVec2 p2 = ImVec2(p1.x + control_offset_x, p1.y);
        const ImVec2 p3 = ImVec2(p4.x - control_offset_x, p4.y);

        draw_list->AddBezierCubic(p1, p2, p3, p4, color, line_thickness, GUI_ROUTING_CURVE_SEGMENTS);
    }
}

void GUIRoutingPanel::draw_all_links() {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    for (const auto &[source, target] : nodes_links) {
        if (target.target == Target::SPLITTER_OUTPUT) {
            const size_t output_channels = song.get_output_channels();
            std::array<uint8_t, MAX_OUTPUT_CHANNELS> splitter = OutputType::unpack_splitter_data(target.index);
            for (size_t i = 0; i < output_channels; ++i) {
                const uint16_t offset = sizeof(_Float32) * i;
                const OutputKey target_key = {Target::DIRECT_OUTPUT, static_cast<int>(i), offset};
                draw_link(source, target_key, splitter[i]);
            }
        } else if (target.target == Target::SPLITTER_DSP) {
            std::array<uint8_t, MAX_OUTPUT_CHANNELS> splitter = OutputType::unpack_splitter_data(target.index);
            int start = target.offset / sizeof(_Float32);
            int end = std::clamp(static_cast<int>(dsps.size()) - start, 0, MAX_OUTPUT_CHANNELS);
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
}

void GUIRoutingPanel::draw_node(RoutingNode &routing_node, const ImVec2 node_rect_min) {
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

    const uint8_t node_alpha = routing_node.bypass ? 128 : 255;
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
    if (!dragging_node_id.has_value() && is_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
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

        const bool is_pin_hovered = ImGui::IsMouseHoveringRect(pin_position - ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS), pin_position + ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS));
        set_source_key(pin_position, key);
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

        input_pins[key] = pin_position;
        set_target_key(pin_position, key);
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

RoutingNode *GUIRoutingPanel::handle_node_dragging(const ImVec2 &canvas_origin) {
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

void GUIRoutingPanel::set_source_key(const ImVec2 pin_position, const InputKey &key) {
    if (gui.is_playing()) {
        return;
    }

    const ImVec2 radius = ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS);
    const bool is_pin_hovered = ImGui::IsMouseHoveringRect(pin_position - radius, pin_position + radius);
    if (is_pin_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        link_dragging_source_key = key;
    } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        link_dragging_source_key = std::nullopt;
    }
}

void GUIRoutingPanel::set_target_key(const ImVec2 pin_position, const OutputKey &key) {
    if (gui.is_playing()) {
        return;
    }

    const ImVec2 radius = ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS);
    const bool is_pin_hovered = ImGui::IsMouseHoveringRect(pin_position - radius, pin_position + radius);
    if (link_dragging_source_key.has_value() && is_pin_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        const InputKey source_key = link_dragging_source_key.value();
        nodes_links[source_key] = key;
        link_dragging_source_key = std::nullopt;
    }
}

bool GUIRoutingPanel::is_linking_possible(const InputKey &source_key, const OutputKey &target_key) const {
    const ItemType type = source_key.first;
    const size_t id = source_key.second;
    const size_t item = target_key.index;
    const Target target = target_key.target;
    return (item > id || type != ItemType::DSP || (target != Target::DIRECT_DSP && target != Target::SPLITTER_DSP));
}

void GUIRoutingPanel::check_keyboard_input() {
}

bool GUIRoutingPanel::get_splitter_bounds(const size_t j, size_t index, const Link &link) const {
    if (link.target != Target::SPLITTER_OUTPUT && link.target != Target::SPLITTER_DSP) {
        return true;
    }

    if (link.target == Target::SPLITTER_OUTPUT) {
        if (j >= song.get_output_channels()) {
            return true;
        }
    } else if (link.target == Target::SPLITTER_DSP) {
        const size_t link_offset = link.offset / sizeof(uint32_t);
        const size_t offset = link.type == ItemType::DSP ? std::max(index + 1, link_offset) : link_offset;
        if (j + offset >= dsps.size()) {
            return true;
        }
    }

    return false;
}

std::vector<std::pair<NodeIdentifier, ImVec2>> GUIRoutingPanel::get_nodes_positions() const {
    std::vector<std::pair<NodeIdentifier, ImVec2>> nodes_positions;
    nodes_positions.reserve(nodes.size());

    for (const RoutingNode &node : nodes) {
        nodes_positions.emplace_back(node.identifier, node.position);
    }

    return nodes_positions;
}

void GUIRoutingPanel::set_nodes_positions(const std::vector<std::pair<NodeIdentifier, ImVec2>> &nodes_positions) {
    for (const auto &[identifier, position] : nodes_positions) {
        for (RoutingNode &node : nodes) {
            if (node.identifier == identifier) {
                node.position = position;
                break;
            }
        }
    }
}
