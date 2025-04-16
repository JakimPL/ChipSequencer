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
    const float dsp_x = channel_x + node_width + 50.0f;
    const float output_x = dsp_x + node_width + 50.0f;

    const auto &channel_routing = routing_variables.at(Target::CHANNEL);
    for (size_t i = 0; i < channels.size(); ++i) {
        NodeInfo channel_node;
        channel_node.id = static_cast<int>(i);
        channel_node.type = Target::CHANNEL;
        channel_node.name = channel_names[i];
        channel_node.position = {channel_x, 0.0f};
        for (const auto &label : channel_routing.labels) {
            channel_node.parameters.push_back(label);
            channel_node.lines += 1;
        }
        nodes.push_back(channel_node);
    }

    const auto &dsp_routing = routing_variables.at(Target::DSP);
    for (size_t i = 0; i < dsps.size(); ++i) {
        NodeInfo dsp_node;
        dsp_node.id = static_cast<int>(i);
        dsp_node.type = Target::DSP;
        dsp_node.name = dsp_names[i];
        dsp_node.position = {dsp_x, 0.0f};
        const DSP *dsp = static_cast<DSP *>(dsps[i]);
        const auto labels = dsp_routing.filter_items(dsp->effect_index).second;
        for (const auto &label : labels) {
            dsp_node.parameters.push_back(label);
            dsp_node.lines += 1;
        }

        nodes.push_back(dsp_node);
    }

    const size_t output_channels = song.get_output_channels();
    for (size_t i = 0; i < output_channels; ++i) {
        NodeInfo output_node;
        output_node.id = static_cast<int>(i);
        output_node.type = Target::OUTPUT_CHANNEL;
        output_node.name = "Output Channel " + std::to_string(i);
        output_node.position = {output_x, 0.0f};
        nodes.push_back(output_node);
    }
}

void GUIRoutingPanel::draw_nodes() {
    panel_scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
    const ImVec2 canvas_origin = ImGui::GetCursorScreenPos();
    const ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    const ImVec2 canvas_p1 = ImVec2(canvas_origin.x + canvas_sz.x, canvas_origin.y + canvas_sz.y);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRect(canvas_origin, canvas_p1, true);

    std::map<float, std::vector<NodeInfo *>> nodes_by_column;
    for (auto &node : nodes) {
        nodes_by_column[node.position.x].push_back(&node);
    }

    const float vertical_padding = 10.0f;
    const float initial_y_offset = 20.0f;

    for (auto &pair : nodes_by_column) {
        float current_y = initial_y_offset;
        const float column_x = pair.first;
        const std::vector<NodeInfo *> &column_nodes = pair.second;

        for (NodeInfo *node_ptr : column_nodes) {
            NodeInfo &node = *node_ptr;
            node.position.y = current_y;
            const ImVec2 node_draw_pos = ImVec2(
                canvas_origin.x + node.position.x - panel_scroll.x,
                canvas_origin.y + node.position.y - panel_scroll.y
            );

            draw_node(node, draw_list, node_draw_pos);
            current_y += node.size.y + vertical_padding;
        }
    }

    // draw_all_links();

    draw_list->PopClipRect();

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
        const ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
        panel_scroll.x -= mouse_delta.x;
        panel_scroll.y -= mouse_delta.y;
        ImGui::SetScrollX(panel_scroll.x);
        ImGui::SetScrollY(panel_scroll.y);
    }
}

void GUIRoutingPanel::draw_node(NodeInfo &node_info, ImDrawList *draw_list, const ImVec2 node_rect_min) {
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
        for (const auto &param : node_info.parameters) {
            max_param_width = std::max(max_param_width, ImGui::CalcTextSize(param.c_str()).x);
        }
    }
    const float content_width = std::max(name_width, max_param_width);
    const float node_actual_width = std::max(node_width, content_width + node_padding_x * 2.0f);

    const ImVec2 node_rect_max = {node_rect_min.x + node_actual_width, node_rect_min.y + node_actual_height};
    node_info.size = {node_actual_width, node_actual_height};

    bool is_hovered = ImGui::IsMouseHoveringRect(node_rect_min, node_rect_max);
    ImU32 node_bg_color = is_hovered ? IM_COL32(70, 70, 70, 200) : IM_COL32(50, 50, 50, 200);
    draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
    draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

    const float pin_radius = 4.0f;
    const ImU32 pin_color_main = IM_COL32(255, 255, 255, 255);
    const ImU32 pin_color_param = IM_COL32(150, 150, 150, 255);
    const float pin_offset_y = line_height * 0.5f;

    ImVec2 current_text_pos = ImVec2(node_rect_min.x + node_padding_x, node_rect_min.y + node_padding_y);

    float pin_y = current_text_pos.y + pin_offset_y;
    draw_list->AddCircleFilled(ImVec2(node_rect_min.x - pin_radius, pin_y), pin_radius, pin_color_main);
    if (node_info.type != Target::OUTPUT_CHANNEL) {
        draw_list->AddCircleFilled(ImVec2(node_rect_max.x + pin_radius, pin_y), pin_radius, pin_color_main);
    }
    draw_list->AddText(current_text_pos, ImGui::GetColorU32(ImGuiCol_Text), node_info.name.c_str());
    current_text_pos.y += line_height + style.ItemSpacing.y;

    if (!node_info.parameters.empty()) {
        for (const auto &param : node_info.parameters) {
            pin_y = current_text_pos.y + pin_offset_y;
            draw_list->AddCircleFilled(ImVec2(node_rect_min.x - pin_radius, pin_y), pin_radius, pin_color_param);
            draw_list->AddCircleFilled(ImVec2(node_rect_max.x + pin_radius, pin_y), pin_radius, pin_color_param);
            draw_list->AddText(current_text_pos, ImGui::GetColorU32(ImGuiCol_Text), param.c_str());
            current_text_pos.y += line_height + style.ItemSpacing.y;
        }
    }

    ImGui::PopID();
}

void GUIRoutingPanel::draw() {
    ImGui::Begin("Routing Editor");
    ImGui::BeginChild("RoutingCanvas", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    from();
    draw_nodes();
    check_keyboard_input();
    to();

    ImGui::EndChild();
    ImGui::End();
}

void GUIRoutingPanel::check_keyboard_input() {
}
