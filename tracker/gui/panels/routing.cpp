#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

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
    update();
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
    nodes.clear();

    const float channel_x = 50.0f;
    const float dsp_x = channel_x + GUI_ROUTING_NODE_WIDTH + 50.0f;
    const float output_x = dsp_x + GUI_ROUTING_NODE_WIDTH + 50.0f;

    const auto &channel_routing = routing_variables.at(Target::CHANNEL);
    for (size_t i = 0; i < channels.size(); ++i) {
        RoutingNode channel_node;
        channel_node.id = i;
        channel_node.key = {ItemType::CHANNEL, static_cast<int>(i)};
        channel_node.type = Target::CHANNEL;
        channel_node.name = channel_names[i];
        channel_node.position = {channel_x, 25.0f};

        for (size_t j = 0; j < channel_routing.labels.size(); ++j) {
            const auto &label = channel_routing.labels[j];
            const auto &offset = channel_routing.offsets[j];
            const OutputKey key = {Target::CHANNEL, static_cast<int>(i), offset};
            channel_node.parameters.push_back({key, label});
            channel_node.lines += 1;
        }

        nodes.push_back(channel_node);
    }

    const auto &dsp_routing = routing_variables.at(Target::DSP);
    for (size_t i = 0; i < dsps.size(); ++i) {
        RoutingNode dsp_node;
        dsp_node.id = i;
        dsp_node.key = {ItemType::DSP, static_cast<int>(i)};
        dsp_node.type = Target::DSP;
        dsp_node.name = dsp_names[i];
        dsp_node.position = {dsp_x, 0.0f};

        const DSP *dsp = static_cast<DSP *>(dsps[i]);
        const auto filtered_items = dsp_routing.filter_items(dsp->effect_index);
        const auto labels = std::get<1>(filtered_items);
        const auto offsets = std::get<2>(filtered_items);
        for (size_t j = 0; j < labels.size(); ++j) {
            const auto &label = labels[j];
            const auto &offset = offsets[j];
            const OutputKey key = {Target::DSP, static_cast<int>(i), offsets[j]};
            dsp_node.parameters.push_back({key, label});
            dsp_node.lines += 1;
        }

        nodes.push_back(dsp_node);
    }

    const size_t output_channels = song.get_output_channels();
    for (size_t i = 0; i < output_channels; ++i) {
        RoutingNode output_node;
        output_node.id = i;
        output_node.type = Target::OUTPUT_CHANNEL;
        output_node.name = "Output Channel " + std::to_string(i);
        output_node.position = {output_x, 25.0f};
        nodes.push_back(output_node);
    }
}

void GUIRoutingPanel::draw_nodes() {
    input_pins.clear();
    output_pins.clear();

    panel_scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
    const ImVec2 canvas_origin = ImGui::GetCursorScreenPos();
    const ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    const ImVec2 canvas_p1 = ImVec2(canvas_origin.x + canvas_sz.x, canvas_origin.y + canvas_sz.y);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRect(canvas_origin, canvas_p1, true);

    std::map<float, std::vector<RoutingNode *>> nodes_by_column;
    for (auto &node : nodes) {
        nodes_by_column[node.position.x].push_back(&node);
    }

    const float vertical_padding = 10.0f;
    const float initial_y_offset = 20.0f;

    for (auto &pair : nodes_by_column) {
        float current_y = initial_y_offset;
        const float column_x = pair.first;
        const std::vector<RoutingNode *> &column_nodes = pair.second;

        for (RoutingNode *node_ptr : column_nodes) {
            RoutingNode &node = *node_ptr;
            node.position.y += current_y;
            const ImVec2 node_draw_pos = ImVec2(
                canvas_origin.x + node.position.x - panel_scroll.x,
                canvas_origin.y + node.position.y - panel_scroll.y
            );

            draw_node(node, node_draw_pos);
            current_y += node.size.y + vertical_padding;
        }
    }

    draw_list->PopClipRect();

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
        const ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
        panel_scroll.x -= mouse_delta.x;
        panel_scroll.y -= mouse_delta.y;
        ImGui::SetScrollX(panel_scroll.x);
        ImGui::SetScrollY(panel_scroll.y);
    }
}

void GUIRoutingPanel::draw_all_links() {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const float line_thickness = 1.5f;
    const ImU32 audio_color = IM_COL32(0, 200, 0, 255);
    const ImU32 parameter_color = IM_COL32(200, 150, 0, 255);

    for (const auto &[source, target] : nodes_links) {
        auto source_pin_it = output_pins.find(source);
        auto dest_pin_it = input_pins.find(target);

        if (source_pin_it != output_pins.end() && dest_pin_it != input_pins.end()) {
            const ImVec2 &p1 = source_pin_it->second;
            const ImVec2 &p4 = dest_pin_it->second;

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

void GUIRoutingPanel::draw_node(RoutingNode &node_info, const ImVec2 node_rect_min) {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    std::string id_prefix;
    switch (node_info.type) {
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
    const std::string id_str = id_prefix + std::to_string(node_info.id);
    ImGui::PushID(id_str.c_str());

    const ImGuiStyle &style = ImGui::GetStyle();
    const float node_padding_x = style.WindowPadding.x;
    const float node_padding_y = style.WindowPadding.y / 2.0f;
    const float line_height = ImGui::GetTextLineHeight();

    const float node_content_height = node_info.lines * line_height + (node_info.lines > 1 ? (node_info.lines - 1) * style.ItemSpacing.y : 0);
    const float node_actual_height = node_content_height + node_padding_y * 2.0f;

    const float name_width = ImGui::CalcTextSize(node_info.name.c_str()).x;
    float max_param_width = 0.0f;
    if (!node_info.parameters.empty()) {
        for (const auto &[parameter_key, parameter_label] : node_info.parameters) {
            max_param_width = std::max(max_param_width, ImGui::CalcTextSize(parameter_label.c_str()).x);
        }
    }
    const float content_width = std::max(name_width, max_param_width);
    const float node_actual_width = std::max(GUI_ROUTING_NODE_WIDTH, content_width + node_padding_x * 2.0f);

    const ImVec2 node_rect_max = {node_rect_min.x + node_actual_width, node_rect_min.y + node_actual_height};
    node_info.size = {node_actual_width, node_actual_height};

    bool is_hovered = ImGui::IsMouseHoveringRect(node_rect_min, node_rect_max);
    ImU32 node_bg_color = is_hovered ? IM_COL32(70, 70, 70, 200) : IM_COL32(50, 50, 50, 200);
    draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
    draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

    const ImU32 pin_color_main = IM_COL32(255, 255, 255, 255);
    const ImU32 pin_color_param = IM_COL32(150, 150, 150, 255);
    const float pin_offset_y = line_height * 0.5f;

    ImVec2 current_text_pos = ImVec2(node_rect_min.x + node_padding_x, node_rect_min.y + node_padding_y);

    float pin_y = current_text_pos.y + pin_offset_y;
    if (node_info.type != Target::CHANNEL) {
        const ImVec2 pin_position = ImVec2(node_rect_min.x - GUI_ROUTING_PIN_RADIUS, pin_y);
        draw_list->AddCircleFilled(pin_position, GUI_ROUTING_PIN_RADIUS, pin_color_main);
        const int index = node_info.id;
        const uint16_t offset = sizeof(_Float32) * node_info.id;
        const Target target = node_info.type == Target::DSP ? Target::DSP_CHANNEL : Target::OUTPUT_CHANNEL;
        input_pins[{target, index, offset}] = pin_position;
    }

    if (node_info.type != Target::OUTPUT_CHANNEL) {
        const ImVec2 pin_position = ImVec2(node_rect_max.x + GUI_ROUTING_PIN_RADIUS, pin_y);
        draw_list->AddCircleFilled(pin_position, GUI_ROUTING_PIN_RADIUS, pin_color_main);
        output_pins[node_info.key.value()] = pin_position;
    }

    draw_list->AddText(current_text_pos, ImGui::GetColorU32(ImGuiCol_Text), node_info.name.c_str());
    current_text_pos.y += line_height + style.ItemSpacing.y;

    for (const auto &parameter : node_info.parameters) {
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

void GUIRoutingPanel::check_keyboard_input() {
}
