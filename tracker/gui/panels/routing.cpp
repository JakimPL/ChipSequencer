#include <algorithm>
#include <cmath>
#include <string>

#include "../../general.hpp"
#include "../../maps/routing.hpp"
#include "../../structures/channel.hpp"
#include "../constants.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "routing.hpp"

GUIRoutingPanel::GUIRoutingPanel() {
    update();
}

void GUIRoutingPanel::update() {
}

void GUIRoutingPanel::add() {
}

void GUIRoutingPanel::remove() {
}

void GUIRoutingPanel::from() {
}

void GUIRoutingPanel::to() const {
}

void GUIRoutingPanel::check_keyboard_input() {
}

void GUIRoutingPanel::collect_nodes() {
    nodes.clear();

    float current_x = 50.0f;
    float current_y = 50.0f;
    const float y_spacing = 50.0f;
    const float default_node_height_estimate = 100.0f;

    for (size_t i = 0; i < channels.size(); ++i) {
        NodeInfo channel_node;
        channel_node.id = static_cast<int>(i);
        channel_node.type = ItemType::CHANNEL;
        channel_node.name = channel_names[i];
        channel_node.pos = {current_x, current_y};

        LinkKey pitch_key = {Target::CHANNEL, static_cast<uint8_t>(i), CHANNEL_PITCH};
        channel_node.input_pins[pitch_key] = {};

        LinkKey out_key = {Target::OUTPUT_CHANNEL, static_cast<uint8_t>(i), CHANNEL_OUTPUT};
        channel_node.output_pins[out_key] = {};

        nodes.push_back(channel_node);

        float node_height = ImGui::GetTextLineHeightWithSpacing() * (2 + channel_node.input_pins.size() + channel_node.output_pins.size());
        current_y += std::max(default_node_height_estimate, node_height) + y_spacing;
    }
}

void GUIRoutingPanel::draw_node(NodeInfo &node_info, ImDrawList *draw_list) {
    std::string id_str = "ChannelNode_" + std::to_string(node_info.id);
    ImGui::PushID(id_str.c_str());

    ImVec2 node_pos_absolute = ImGui::GetCursorScreenPos();
    ImVec2 node_rect_min = {node_pos_absolute.x + node_info.pos.x - panel_scroll.x, node_pos_absolute.y + node_info.pos.y - panel_scroll.y};

    ImGui::SetCursorScreenPos(node_rect_min);

    ImGui::BeginGroup();
    ImGui::TextUnformatted(node_info.name.c_str());
    ImGui::Spacing();
    ImGui::EndGroup();

    ImVec2 node_content_size = ImGui::GetItemRectSize();
    float node_actual_width = std::max(node_width, node_content_size.x);
    float node_title_height = ImGui::GetTextLineHeightWithSpacing();

    float input_pin_height = node_info.input_pins.empty() ? 0 : (node_info.input_pins.size() * ImGui::GetTextLineHeightWithSpacing());
    float output_pin_height = node_info.output_pins.empty() ? 0 : (node_info.output_pins.size() * ImGui::GetTextLineHeightWithSpacing());
    float required_pin_height = std::max(input_pin_height, output_pin_height);
    float node_actual_height = std::max(node_content_size.y, node_title_height + required_pin_height) + ImGui::GetStyle().ItemSpacing.y;

    ImVec2 node_rect_max = {node_rect_min.x + node_actual_width, node_rect_min.y + node_actual_height};
    draw_list->AddRectFilled(node_rect_min, node_rect_max, IM_COL32(50, 50, 50, 200), 4.0f);
    draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

    float current_pin_y = node_rect_min.y + node_title_height;
    const float pin_spacing = ImGui::GetTextLineHeightWithSpacing();

    for (auto it = node_info.input_pins.begin(); it != node_info.input_pins.end(); ++it) {
        ImVec2 pin_pos = {node_rect_min.x - pin_radius, current_pin_y + pin_spacing * 0.5f};
        it->second = pin_pos;
        draw_list->AddCircleFilled(pin_pos, pin_radius, IM_COL32(150, 150, 150, 255));
        current_pin_y += pin_spacing;
    }

    current_pin_y = node_rect_min.y + node_title_height;
    for (auto it = node_info.output_pins.begin(); it != node_info.output_pins.end(); ++it) {
        ImVec2 pin_pos = {node_rect_max.x + pin_radius, current_pin_y + pin_spacing * 0.5f};
        it->second = pin_pos;
        draw_list->AddCircleFilled(pin_pos, pin_radius, IM_COL32(200, 200, 200, 255));
        current_pin_y += pin_spacing;
    }

    node_info.size = {node_actual_width, node_actual_height};

    ImGui::PopID();
}

void GUIRoutingPanel::draw() {
    ImGui::Begin("Routing Editor", &visible);

    ImGui::BeginChild("RoutingCanvas", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    panel_scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRect(canvas_p0, canvas_p1, true);

    collect_nodes();

    for (auto &node : nodes) {
        draw_node(node, draw_list);
    }

    draw_list->PopClipRect();

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
        ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
        panel_scroll.x -= mouse_delta.x;
        panel_scroll.y -= mouse_delta.y;
        ImGui::SetScrollX(panel_scroll.x);
        ImGui::SetScrollY(panel_scroll.y);
    }

    ImGui::EndChild();
    ImGui::End();
}
