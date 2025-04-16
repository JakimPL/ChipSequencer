#ifndef GUI_PANELS_ROUTING_HPP
#define GUI_PANELS_ROUTING_HPP

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

    RoutingNode *dragging_node = nullptr;
    ImVec2 drag_node_offset = {0.0f, 0.0f};

    void collect_links();
    void collect_nodes();
    void draw_node(RoutingNode &routing_node, const ImVec2 node_rect_min);
    void draw_nodes();
    void draw_all_links();

    void add() override {};
    void remove() override {};
    void from() override;
    void to() const override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIRoutingPanel();
    void update() override;
};

#endif // GUI_PANELS_ROUTING_HPP
