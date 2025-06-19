#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "../imgui/imgui.h"

#include "../node.hpp"
#include "panel.hpp"

typedef std::array<uint8_t, MAX_OUTPUT_CHANNELS> Splitter;

class GUIRoutingsPanel : public GUIPanel {
  private:
    std::vector<RoutingNode> nodes;
    std::map<InputKey, OutputKey> nodes_links;
    std::map<OutputKey, ImVec2> input_pins;
    std::map<InputKey, ImVec2> output_pins;

    std::optional<InputKey> link_dragging_source_key;
    std::optional<NodeIdentifier> dragging_node_id;
    ImVec2 drag_node_offset = {0.0f, 0.0f};

    void collect_nodes();
    void collect_links();
    void to_nodes() const;
    void to_links() const;

    void draw_reset_button();
    void draw_node(RoutingNode &routing_node, const ImVec2 node_rect_min);
    void draw_nodes();
    void draw_link(const InputKey &source_key, const OutputKey &target_key, uint8_t alpha = UINT8_MAX);
    void draw_all_links();
    ImVec2 calculate_content_size() const;

    void reset_dragging();
    void reset_linking();
    bool is_linking_possible(const InputKey &source_key, const OutputKey &target_key) const;
    RoutingNode *handle_node_dragging(const ImVec2 &canvas_origin);
    void set_source_key(const ImVec2 pin_position, const InputKey &source_key);
    void set_target_key(const ImVec2 pin_position, const OutputKey &target_key);

    void update_channel_node(size_t index, RoutingNode &channel_node);
    void update_dsp_node(size_t index, RoutingNode &dsp_nodes);
    void update_output_node(size_t index, RoutingNode &output_node);
    void add_channel_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);
    void add_dsp_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);
    void add_output_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);

    bool get_splitter_bounds(const size_t j, size_t index, const Link &link) const;
    Splitter get_splitter_from_input_key(const InputKey &source) const;

    bool is_disabled() const override;
    void draw() override;

  public:
    GUIRoutingsPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;

    void from() override;
    void to() const override;

    void clear_nodes();
    void set_link(const InputKey &source_key, const OutputKey &target_key);
    std::vector<std::pair<NodeIdentifier, ImVec2>> get_nodes_positions() const;
    void set_nodes_positions(const std::vector<std::pair<NodeIdentifier, ImVec2>> &nodes_positions);
};
