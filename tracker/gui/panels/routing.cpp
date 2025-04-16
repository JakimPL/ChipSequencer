#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

#include <iostream>

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

GUIRoutingPanel::GUIRoutingPanel() {
}

void GUIRoutingPanel::update() {
}

void GUIRoutingPanel::from() {
    collect_nodes();
    collect_links();
}

void GUIRoutingPanel::to() const {
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
            update_channel_node(i, it->second, next_nodes);
            existing_node_indices.erase(it);
        } else {
            add_channel_node(i, next_nodes, column_next_y);
        }
    }

    for (size_t i = 0; i < dsps.size(); ++i) {
        NodeIdentifier current_id = {Target::DSP, i};
        auto it = existing_node_indices.find(current_id);

        if (it != existing_node_indices.end()) {
            update_dsp_node(i, it->second, next_nodes);
            existing_node_indices.erase(it);
        } else {
            add_dsp_node(i, next_nodes, column_next_y);
        }
    }

    const size_t output_channels_count = song.get_output_channels();
    for (size_t i = 0; i < output_channels_count; ++i) {
        NodeIdentifier current_id = {Target::OUTPUT_CHANNEL, i};
        auto it = existing_node_indices.find(current_id);

        if (it != existing_node_indices.end()) {
            update_output_node(i, it->second, next_nodes);
            existing_node_indices.erase(it);
        } else {
            add_output_node(i, next_nodes, column_next_y);
        }
    }

    nodes = std::move(next_nodes);
}

void GUIRoutingPanel::update_channel_node(size_t index, size_t existing_index, std::vector<RoutingNode> &next_nodes) {
    RoutingNode &node_to_update = nodes[existing_index];
    const auto &channel_routing = routing_variables.at(Target::CHANNEL);

    node_to_update.name = channel_names[index];
    node_to_update.parameters.clear();
    node_to_update.lines = 1;
    for (size_t j = 0; j < channel_routing.labels.size(); ++j) {
        const OutputKey key = {Target::CHANNEL, static_cast<int>(index), channel_routing.offsets[j]};
        node_to_update.parameters.push_back({key, channel_routing.labels[j]});
        node_to_update.lines += 1;
    }
    next_nodes.push_back(node_to_update);
}

void GUIRoutingPanel::add_channel_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = 50.0f;
    const float vertical_padding = 20.0f;
    const auto &channel_routing = routing_variables.at(Target::CHANNEL);
    NodeIdentifier current_id = {Target::CHANNEL, index};

    RoutingNode channel_node;
    channel_node.identifier = current_id;
    channel_node.key = {ItemType::CHANNEL, static_cast<int>(index)};
    channel_node.name = channel_names[index];
    for (size_t j = 0; j < channel_routing.labels.size(); ++j) {
        const OutputKey key = {Target::CHANNEL, static_cast<int>(index), channel_routing.offsets[j]};
        channel_node.parameters.push_back({key, channel_routing.labels[j]});
        channel_node.lines += 1;
    }

    channel_node.position = {channel_x, column_next_y[channel_x]};
    float estimated_height = channel_node.lines * ImGui::GetTextLineHeight();
    column_next_y[channel_x] += estimated_height + vertical_padding;

    next_nodes.push_back(channel_node);
}

void GUIRoutingPanel::update_dsp_node(size_t index, size_t existing_index, std::vector<RoutingNode> &next_nodes) {
    RoutingNode &node_to_update = nodes[existing_index];
    const auto &dsp_routing = routing_variables.at(Target::DSP);

    node_to_update.name = dsp_names[index];
    node_to_update.parameters.clear();
    node_to_update.lines = 1;
    const DSP *dsp = static_cast<DSP *>(dsps[index]);
    const auto filtered_items = dsp_routing.filter_items(dsp->effect_index);
    const auto labels = std::get<1>(filtered_items);
    const auto offsets = std::get<2>(filtered_items);
    for (size_t j = 0; j < labels.size(); ++j) {
        const OutputKey key = {Target::DSP, static_cast<int>(index), offsets[j]};
        node_to_update.parameters.push_back({key, labels[j]});
        node_to_update.lines += 1;
    }
    next_nodes.push_back(node_to_update);
}

void GUIRoutingPanel::add_dsp_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = 50.0f;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float vertical_padding = 20.0f;
    const auto &dsp_routing = routing_variables.at(Target::DSP);
    NodeIdentifier current_id = {Target::DSP, index};

    RoutingNode dsp_node;
    dsp_node.identifier = current_id;
    dsp_node.key = {ItemType::DSP, static_cast<int>(index)};
    dsp_node.name = dsp_names[index];
    const DSP *dsp = static_cast<DSP *>(dsps[index]);
    const auto filtered_items = dsp_routing.filter_items(dsp->effect_index);
    const auto labels = std::get<1>(filtered_items);
    const auto offsets = std::get<2>(filtered_items);
    for (size_t j = 0; j < labels.size(); ++j) {
        const OutputKey key = {Target::DSP, static_cast<int>(index), offsets[j]};
        dsp_node.parameters.push_back({key, labels[j]});
        dsp_node.lines += 1;
    }

    dsp_node.position = {dsp_x, column_next_y[dsp_x]};
    float estimated_height = dsp_node.lines * ImGui::GetTextLineHeight();
    column_next_y[dsp_x] += estimated_height + vertical_padding;

    next_nodes.push_back(dsp_node);
}

void GUIRoutingPanel::update_output_node(size_t index, size_t existing_index, std::vector<RoutingNode> &next_nodes) {
    RoutingNode &node_to_update = nodes[existing_index];
    node_to_update.name = "Output Channel " + std::to_string(index);
    node_to_update.parameters.clear();
    node_to_update.lines = 1;
    next_nodes.push_back(node_to_update);
}

void GUIRoutingPanel::add_output_node(size_t index, std::vector<RoutingNode> &next_nodes, std::map<float, float> &column_next_y) {
    const float channel_x = 50.0f;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float output_x = dsp_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float vertical_padding = 20.0f;
    NodeIdentifier current_id = {Target::OUTPUT_CHANNEL, index};

    RoutingNode output_node;
    output_node.identifier = current_id;
    output_node.name = "Output Channel " + std::to_string(index);

    output_node.position = {output_x, column_next_y[output_x]};
    float estimated_height = output_node.lines * ImGui::GetTextLineHeight();
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

void GUIRoutingPanel::draw_all_links() {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const float line_thickness = 1.5f;
    const ImU32 audio_color = IM_COL32(0, 200, 0, 255);
    const ImU32 parameter_color = IM_COL32(200, 150, 0, 255);

    for (const auto &[source, target] : nodes_links) {
        const bool dragging = link_dragging_source_key.has_value() && link_dragging_source_key.value() == source;
        auto source_pin_it = output_pins.find(source);
        auto dest_pin_it = input_pins.find(target);

        if (source_pin_it != output_pins.end() && dest_pin_it != input_pins.end()) {
            ImVec2 p1, p4;
            if (dragging) {
                const ImVec2 mouse_pos = ImGui::GetMousePos();
                p1 = source_pin_it->second;
                p4 = mouse_pos;
            } else {
                p1 = source_pin_it->second;
                p4 = dest_pin_it->second;
            }

            const float horizontal_distance = std::abs(p4.x - p1.x);
            const float control_offset_x = std::max(30.0f, horizontal_distance * 0.4f);

            const ImVec2 p2 = ImVec2(p1.x + control_offset_x, p1.y);
            const ImVec2 p3 = ImVec2(p4.x - control_offset_x, p4.y);

            const bool is_audio_link = target.target == Target::OUTPUT_CHANNEL || target.target == Target::DSP_CHANNEL;
            const ImU32 color = is_audio_link ? audio_color : parameter_color;
            draw_list->AddBezierCubic(p1, p2, p3, p4, color, line_thickness, GUI_ROUTING_CURVE_SEGMENTS);
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
    case Target::DSP:
    case Target::DSP_CHANNEL:
        id_prefix = "DSPNode_";
        break;
    case Target::OUTPUT_CHANNEL:
        id_prefix = "OutputNode_";
        break;
    case Target::ENVELOPE:
        id_prefix = "EnvelopeNode_";
        break;
    case Target::SEQUENCE:
        id_prefix = "SequenceNode_";
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
    case Target::UNUSED:
    default:
        id_prefix = "UnknownNode_";
        break;
    }
    const std::string id_str = id_prefix + std::to_string(routing_node.identifier.id);
    ImGui::PushID(id_str.c_str());

    const ImGuiStyle &style = ImGui::GetStyle();
    const float node_padding_x = style.WindowPadding.x;
    const float node_padding_y = style.WindowPadding.y / 2.0f;
    const float line_height = ImGui::GetTextLineHeight();

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
    }

    ImU32 node_bg_color = is_hovered ? IM_COL32(70, 70, 70, 200) : IM_COL32(50, 50, 50, 200);
    draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
    draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

    const ImU32 pin_color_main = IM_COL32(255, 255, 255, 255);
    const ImU32 pin_color_param = IM_COL32(150, 150, 150, 255);
    const float pin_offset_y = line_height * 0.5f;

    ImVec2 current_text_pos = ImVec2(node_rect_min.x + node_padding_x, node_rect_min.y + node_padding_y);
    float pin_y = current_text_pos.y + pin_offset_y;

    if (routing_node.identifier.type != Target::OUTPUT_CHANNEL) {
        const ImVec2 pin_position = ImVec2(node_rect_max.x + GUI_ROUTING_PIN_RADIUS, pin_y);
        draw_list->AddCircleFilled(pin_position, GUI_ROUTING_PIN_RADIUS, pin_color_main);
        const InputKey key = routing_node.key.value();
        output_pins[key] = pin_position;

        const bool is_pin_hovered = ImGui::IsMouseHoveringRect(pin_position - ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS), pin_position + ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS));
        set_dragging_source_key(pin_position, key);
    }

    if (routing_node.identifier.type != Target::CHANNEL) {
        const ImVec2 pin_position = ImVec2(node_rect_min.x - GUI_ROUTING_PIN_RADIUS, pin_y);
        draw_list->AddCircleFilled(pin_position, GUI_ROUTING_PIN_RADIUS, pin_color_main);
        const int index = routing_node.identifier.id;
        const uint16_t offset = sizeof(_Float32) * routing_node.identifier.id;
        const Target target = routing_node.identifier.type == Target::DSP ? Target::DSP_CHANNEL : Target::OUTPUT_CHANNEL;
        const OutputKey key = {target, index, offset};

        set_dragging_target_key(pin_position, key);
        input_pins[key] = pin_position;
    }

    draw_list->AddText(current_text_pos, ImGui::GetColorU32(ImGuiCol_Text), routing_node.name.c_str());
    current_text_pos.y += line_height + style.ItemSpacing.y;

    for (const auto &parameter : routing_node.parameters) {
        const auto &[parameter_key, parameter_label] = parameter;
        pin_y = current_text_pos.y + pin_offset_y;
        const ImVec2 pin_position = ImVec2(node_rect_min.x - GUI_ROUTING_PIN_RADIUS, pin_y);
        draw_list->AddCircleFilled(pin_position, GUI_ROUTING_PIN_RADIUS, pin_color_param);
        draw_list->AddText(current_text_pos, pin_color_param, parameter_label.c_str());
        current_text_pos.y += line_height + style.ItemSpacing.y;
        input_pins[parameter_key] = pin_position;
    }

    ImGui::PopID();
}

void GUIRoutingPanel::draw() {
    ImGui::Begin("Routing Editor");
    ImGui::BeginChild("RoutingCanvas", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    from();
    draw_nodes();
    draw_all_links();
    check_keyboard_input();
    to();

    ImGui::EndChild();
    ImGui::End();
}

RoutingNode *GUIRoutingPanel::handle_node_dragging(const ImVec2 &canvas_origin) {
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

void GUIRoutingPanel::set_dragging_source_key(const ImVec2 pin_position, const InputKey &key) {
    const bool is_pin_hovered = ImGui::IsMouseHoveringRect(pin_position - ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS), pin_position + ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS));
    if (is_pin_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (link_dragging_source_key.has_value() && link_dragging_source_key.value() == key) {
            link_dragging_source_key = std::nullopt;
        } else {
            link_dragging_source_key = key;
        }
    } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        link_dragging_source_key = std::nullopt;
    }
}

void GUIRoutingPanel::set_dragging_target_key(const ImVec2 pin_position, const OutputKey &key) {
    const bool is_pin_hovered = ImGui::IsMouseHoveringRect(pin_position - ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS), pin_position + ImVec2(GUI_ROUTING_PIN_RADIUS, GUI_ROUTING_PIN_RADIUS));
    if (is_pin_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (link_dragging_source_key.has_value()) {
            const InputKey source_key = link_dragging_source_key.value();
            if (nodes_links.find(source_key) == nodes_links.end()) {
                nodes_links[source_key] = key;
            }
            link_dragging_source_key = std::nullopt;
        }
    }
}

void GUIRoutingPanel::check_keyboard_input() {
}
