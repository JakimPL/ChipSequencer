#ifndef GUI_PANELS_ROUTING_HPP
#define GUI_PANELS_ROUTING_HPP

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "../imgui/imgui.h"

#include "../node.hpp"
#include "panel.hpp"

class GUIRoutingPanel : public GUIPanel {
  private:
    std::vector<RoutingNode> nodes;
    std::map<InputKey, OutputKey> nodes_links;
    std::map<OutputKey, ImVec2> input_pins;
    std::map<InputKey, ImVec2> output_pins;

    std::optional<InputKey> link_dragging_source_key;
    std::optional<NodeIdentifier> dragging_node_id;
    ImVec2 drag_node_offset = {0.0f, 0.0f};

    void collect_links();
    void collect_nodes();
    void draw_node(RoutingNode &routing_node, const ImVec2 node_rect_min);
    void draw_nodes();
    void draw_link(const InputKey source, const OutputKey target, uint8_t alpha = UINT8_MAX);
    void draw_all_links();

    bool is_linking_possible(const InputKey &source_key, const OutputKey &target_key) const;
    RoutingNode *handle_node_dragging(const ImVec2 &canvas_origin);
    void set_dragging_source_key(const ImVec2 pin_position, const InputKey &key);
    void set_dragging_target_key(const ImVec2 pin_position, const OutputKey &key);

    void update_channel_node(size_t index, RoutingNode &channel_node, std::vector<RoutingNode> &nodes);
    void add_channel_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);
    void update_dsp_node(size_t index, RoutingNode &dsp_node, std::vector<RoutingNode> &nodes);
    void add_dsp_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);
    void update_output_node(size_t index, RoutingNode &output_node, std::vector<RoutingNode> &nodes);
    void add_output_node(size_t index, std::vector<RoutingNode> &nodes, std::map<float, float> &column_next_y);

    bool get_splitter_bounds(const size_t j, const Link &link) const;

    void add() override {};
    void duplicate() override {};
    void remove() override {};
    void from() override;
    void to() const override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIRoutingPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override {};
};

#endif // GUI_PANELS_ROUTING_HPP
