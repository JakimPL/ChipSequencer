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

    const float channel_x = 50.0f;
    const float dsp_x = channel_x + node_width + 50.0f;
    const float output_x = dsp_x + node_width + 50.0f;
    const float vertical_spacing = 40.0f;

    const RoutingItems &channel_routing_info = routing_variables.at(Target::CHANNEL);
    for (size_t i = 0; i < channels.size(); ++i) {
        NodeInfo channel_node;
        channel_node.id = static_cast<int>(i);
        channel_node.type = Target::CHANNEL;
        channel_node.name = channel_names[i];
        channel_node.position = {channel_x, vertical_spacing * i};
        for (const uint16_t offset : channel_routing_info.offsets) {
            const LinkKey input_key = {Target::CHANNEL, static_cast<uint8_t>(i), offset};
            channel_node.input_pins[input_key] = {};
        }
        const LinkKey output_key = {Target::CHANNEL, static_cast<uint8_t>(i), CHANNEL_OUTPUT};
        channel_node.output_pins[output_key] = {};
        nodes.push_back(channel_node);
    }

    const auto &dsp_parameter_info = routing_variables.at(Target::DSP);
    for (size_t i = 0; i < dsps.size(); ++i) {
        NodeInfo dsp_node;
        dsp_node.id = static_cast<int>(i);
        dsp_node.type = Target::DSP_CHANNEL;
        dsp_node.name = dsp_names[i];
        dsp_node.position = {dsp_x, vertical_spacing * i};

        const LinkKey dsp_input_key = {Target::DSP_CHANNEL, static_cast<uint8_t>(i), static_cast<uint16_t>(sizeof(_Float32) * i)};
        dsp_node.input_pins[dsp_input_key] = {};

        const DSP *dsp = static_cast<const DSP *>(dsps[i]);
        const int effect_index = dsp->effect_index;
        const auto [filtered_indices, filtered_labels] = dsp_parameter_info.filter_items(effect_index);
        for (const size_t item_index : filtered_indices) {
            const uint16_t offset = dsp_parameter_info.offsets[item_index];
            const LinkKey param_input_key = {Target::DSP, static_cast<uint8_t>(i), offset};
            dsp_node.input_pins[param_input_key] = {};
        }

        const LinkKey output_key = {Target::DSP_CHANNEL, static_cast<uint8_t>(i), static_cast<uint16_t>(sizeof(_Float32) * i)};
        dsp_node.output_pins[output_key] = {};
        nodes.push_back(dsp_node);
    }

    const size_t output_channels = song.get_output_channels();
    for (size_t i = 0; i < output_channels; ++i) {
        NodeInfo output_node;
        output_node.id = static_cast<int>(i);
        output_node.type = Target::OUTPUT_CHANNEL;
        output_node.name = "Output Channel " + std::to_string(i);
        output_node.position = {output_x, vertical_spacing * i};
        const LinkKey input_key = {Target::OUTPUT_CHANNEL, static_cast<uint8_t>(i), static_cast<uint16_t>(sizeof(_Float32) * i)};
        output_node.input_pins[input_key] = {};
        nodes.push_back(output_node);
    }
}

void GUIRoutingPanel::draw_input_pins(NodeInfo &node_info, ImDrawList *draw_list, const ImVec2 node_rect_min, const float node_title_height) {
    size_t regular_pin_index = 0;
    const ImU32 pin_text_color = IM_COL32(200, 200, 200, 255);
    const float pin_label_offset = pin_radius + 3.0f;
    const float pin_spacing = ImGui::GetTextLineHeightWithSpacing();

    for (auto it = node_info.input_pins.begin(); it != node_info.input_pins.end(); ++it) {
        const LinkKey &key = it->first;
        ImVec2 pin_pos;
        std::string pin_label = "";

        if (key.target == Target::DSP_CHANNEL) {
            pin_pos = {node_rect_min.x - pin_radius, node_rect_min.y + node_title_height * 0.5f};
            pin_label = "DSP Input";
        } else {
            const float pin_y_pos = node_rect_min.y + node_title_height + (static_cast<float>(regular_pin_index) + 0.5f) * pin_spacing;
            pin_pos = {node_rect_min.x - pin_radius, pin_y_pos};
            regular_pin_index++;

            if (key.target == Target::DSP) {
                const auto &dsp_param_info = routing_variables.at(Target::DSP);
                auto offset_it = dsp_param_info.offset_to_index.find(key.offset);
                if (offset_it != dsp_param_info.offset_to_index.end() && offset_it->second < dsp_param_info.labels.size()) {
                    pin_label = dsp_param_info.labels[offset_it->second];
                }
            } else if (key.target == Target::CHANNEL) {
                const auto &channel_param_info = routing_variables.at(Target::CHANNEL);
                auto offset_it = channel_param_info.offset_to_index.find(key.offset);
                if (offset_it != channel_param_info.offset_to_index.end() && offset_it->second < channel_param_info.labels.size()) {
                    pin_label = channel_param_info.labels[offset_it->second];
                }
            }
        }

        it->second = pin_pos;
        draw_list->AddCircleFilled(pin_pos, pin_radius, IM_COL32(150, 150, 150, 255));
        if (!pin_label.empty()) {
            ImVec2 text_pos = {pin_pos.x + pin_label_offset, pin_pos.y - ImGui::GetTextLineHeight() * 0.5f};
            draw_list->AddText(text_pos, pin_text_color, pin_label.c_str());
        }
    }
}

void GUIRoutingPanel::draw_output_pins(NodeInfo &node_info, ImDrawList *draw_list, const ImVec2 node_rect_max, const float node_title_height) {
    size_t output_pin_index = 0;
    const float pin_spacing = ImGui::GetTextLineHeightWithSpacing();

    for (auto it = node_info.output_pins.begin(); it != node_info.output_pins.end(); ++it, ++output_pin_index) {
        const float pin_y_pos = node_rect_max.y - node_info.size.y + node_title_height + (static_cast<float>(output_pin_index) + 0.5f) * pin_spacing;
        const ImVec2 pin_pos = {node_rect_max.x + pin_radius, pin_y_pos};
        it->second = pin_pos;
        draw_list->AddCircleFilled(pin_pos, pin_radius, IM_COL32(200, 200, 200, 255));
    }
}

void GUIRoutingPanel::draw_node(NodeInfo &node_info, ImDrawList *draw_list, const ImVec2 node_rect_min) {
    std::string id_prefix;
    switch (node_info.type) {
    case Target::CHANNEL:
        id_prefix = "ChannelNode_";
        break;
    case Target::DSP_CHANNEL:
        id_prefix = "DSPNode_";
        break;
    case Target::OUTPUT_CHANNEL:
        id_prefix = "OutputNode_";
        break;
    case Target::ENVELOPE:
    case Target::SEQUENCE:
    case Target::ORDER:
    case Target::OSCILLATOR:
    case Target::WAVETABLE:
    case Target::DSP:
    case Target::UNUSED:
    default:
        id_prefix = "UnknownNode_";
        break;
    }
    const std::string id_str = id_prefix + std::to_string(node_info.id);
    ImGui::PushID(id_str.c_str());

    ImGui::SetCursorScreenPos(node_rect_min);

    ImGui::BeginGroup();
    ImGui::TextUnformatted(node_info.name.c_str());
    ImGui::Spacing();
    ImGui::EndGroup();

    ImVec2 node_content_size = ImGui::GetItemRectSize();
    const float node_actual_width = std::max(node_width, node_content_size.x);
    const float node_title_height = ImGui::GetTextLineHeightWithSpacing();

    float input_param_pin_count = 0;
    for (const auto &pair : node_info.input_pins) {
        if (!(pair.first.target == Target::DSP_CHANNEL)) {
            input_param_pin_count++;
        }
    }
    const float output_pin_count = static_cast<float>(node_info.output_pins.size());
    const float max_regular_pin_count = std::max(input_param_pin_count, output_pin_count);

    const float pin_spacing = ImGui::GetTextLineHeightWithSpacing();
    const float required_pin_height = max_regular_pin_count * pin_spacing;
    const float node_actual_height = std::max(node_content_size.y, node_title_height + required_pin_height) + ImGui::GetStyle().ItemSpacing.y;

    const ImVec2 node_rect_max = {node_rect_min.x + node_actual_width, node_rect_min.y + node_actual_height};
    draw_list->AddRectFilled(node_rect_min, node_rect_max, IM_COL32(50, 50, 50, 200), 4.0f);
    draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

    node_info.size = {node_actual_width, node_actual_height};

    draw_input_pins(node_info, draw_list, node_rect_min, node_title_height);
    draw_output_pins(node_info, draw_list, node_rect_max, node_title_height);

    node_info.position = {node_rect_min.x - ImGui::GetCursorScreenPos().x + panel_scroll.x, node_rect_min.y - ImGui::GetCursorScreenPos().y + panel_scroll.y};

    ImGui::PopID();
}

void GUIRoutingPanel::draw_all_links(ImDrawList *draw_list, const std::map<std::pair<Target, int>, NodeInfo *> &node_lookup) {
    const ImU32 link_color = IM_COL32(255, 255, 0, 255);
    const float link_thickness = 1.5f;

    auto draw_link_set = [&](Target source_node_type, uint16_t source_pin_offset, const std::vector<Link> &link_set) {
        for (size_t source_id = 0; source_id < link_set.size(); ++source_id) {
            const Link &link = link_set[source_id];
            if (link.target == Target::UNUSED) continue;

            const LinkKey source_key = {source_node_type, static_cast<uint8_t>(source_id), source_pin_offset};
            LinkKey target_key;
            Target target_node_type_lookup = link.target;
            int target_node_id_lookup = static_cast<int>(link.index);

            if (link.target == Target::DSP_CHANNEL) {
                target_key = {Target::DSP_CHANNEL, link.index, static_cast<uint16_t>(sizeof(_Float32) * link.index)};
            } else if (link.target == Target::DSP) {
                target_key = {Target::DSP, link.index, link.offset};
                target_node_type_lookup = Target::DSP_CHANNEL;
            } else {
                target_key = {link.target, link.index, link.offset};
            }

            auto source_node_it = node_lookup.find({source_node_type, static_cast<int>(source_id)});
            auto target_node_it = node_lookup.find({target_node_type_lookup, target_node_id_lookup});

            if (source_node_it != node_lookup.end() && target_node_it != node_lookup.end()) {
                NodeInfo *source_node = source_node_it->second;
                NodeInfo *target_node = target_node_it->second;

                auto source_pin_it = source_node->output_pins.find(source_key);
                auto target_pin_it = target_node->input_pins.find(target_key);

                if (source_pin_it != source_node->output_pins.end() && target_pin_it != target_node->input_pins.end()) {
                    draw_list->AddLine(source_pin_it->second, target_pin_it->second, link_color, link_thickness);
                }
            }
        }
    };

    draw_link_set(Target::CHANNEL, CHANNEL_OUTPUT, links[0]);
    draw_link_set(Target::DSP_CHANNEL, DSP_OUTPUT, links[1]);
}

void GUIRoutingPanel::draw() {
    ImGui::Begin("Routing Editor");

    ImGui::BeginChild("RoutingCanvas", ImVec2(0, 0), false, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    panel_scroll = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
    const ImVec2 canvas_origin = ImGui::GetCursorScreenPos();
    const ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
    const ImVec2 canvas_p1 = ImVec2(canvas_origin.x + canvas_sz.x, canvas_origin.y + canvas_sz.y);

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRect(canvas_origin, canvas_p1, true);

    collect_nodes();

    std::map<float, std::vector<NodeInfo *>> nodes_by_column;
    for (auto &node : nodes) {
        nodes_by_column[node.position.x].push_back(&node);
    }

    const float y_spacing = 20.0f;
    const float initial_y = 50.0f;

    for (auto &pair : nodes_by_column) {
        float current_y = initial_y;
        const std::vector<NodeInfo *> &column_nodes = pair.second;
        for (NodeInfo *node_ptr : column_nodes) {
            NodeInfo &node = *node_ptr;
            const ImVec2 node_draw_pos = {canvas_origin.x + node.position.x - panel_scroll.x, canvas_origin.y + current_y - panel_scroll.y};
            draw_node(node, draw_list, node_draw_pos);
            current_y += node.size.y + y_spacing;
        }
    }

    std::map<std::pair<Target, int>, NodeInfo *> node_lookup;
    for (auto &node : nodes) {
        node_lookup[{node.type, node.id}] = &node;
    }

    draw_all_links(draw_list, node_lookup);

    draw_list->PopClipRect();

    if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)) {
        const ImVec2 mouse_delta = ImGui::GetIO().MouseDelta;
        panel_scroll.x -= mouse_delta.x;
        panel_scroll.y -= mouse_delta.y;
        ImGui::SetScrollX(panel_scroll.x);
        ImGui::SetScrollY(panel_scroll.y);
    }

    ImGui::EndChild();
    ImGui::End();
}
