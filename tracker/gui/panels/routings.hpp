#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "../imgui/imgui.h"

#include "../node.hpp"
#include "panel.hpp"

using Splitter = std::array<uint8_t, MAX_OUTPUT_CHANNELS>;

class GUIRoutingsPanel : public GUIPanel {
  private:
    std::vector<RoutingNode> nodes;
    std::map<InputKey, OutputKey> nodes_links;
    std::map<OutputKey, ImVec2> input_pins;
    std::map<InputKey, ImVec2> output_pins;

    std::optional<InputKey> link_dragging_source_key;
    std::optional<NodeIdentifier> dragging_node_id;
    ImVec2 drag_node_offset = {0.0f, 0.0f};

    bool channel_solo = false;
    bool dsp_solo = false;

    void from_nodes();
    void from_links();
    void to_nodes() const;
    void to_links() const;

    void draw_reset_button();
    void draw_node(RoutingNode &routing_node, ImVec2 node_rect_min);
    void draw_nodes();
    void draw_link(const InputKey &source_key, const OutputKey &target_key, uint8_t alpha = UINT8_MAX);
    void draw_all_links();
    ImVec2 calculate_content_size() const;

    void reset_dragging();
    void reset_linking();
    static bool is_linking_possible(const InputKey &source_key, const OutputKey &target_key);
    RoutingNode *handle_node_dragging(const ImVec2 &canvas_origin);
    void set_source_key(ImVec2 pin_position, const InputKey &source_key);
    void set_target_key(ImVec2 pin_position, const OutputKey &target_key);

    void update_channel_node(size_t index, RoutingNode &channel_node);
    void update_dsp_node(size_t index, RoutingNode &dsp_nodes);
    static void update_output_node(size_t index, RoutingNode &output_node);
    void add_channel_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);
    void add_dsp_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);
    static void add_output_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);

    bool get_bypass_state(const RoutingNode &node) const;
    bool get_bypass_state(ItemType type, bool bypass, bool solo) const;
    static bool get_splitter_bounds(size_t j, size_t index, const Link &link);
    static Splitter get_splitter_from_input_key(const InputKey &source);
    static bool is_node_locked(const RoutingNode &node);
    static bool is_node_locked(InputKey input_key);

    bool is_disabled() const override;
    void draw() override;

  public:
    explicit GUIRoutingsPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void from() override;
    void to() const override;

    void clear_nodes();
    void set_link(const InputKey &source_key, const OutputKey &target_key);
    std::vector<std::pair<NodeIdentifier, ImVec2>> get_nodes_positions() const;
    void set_nodes_positions(const std::vector<std::pair<NodeIdentifier, ImVec2>> &nodes_positions);
};
