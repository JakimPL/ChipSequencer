#ifndef GUI_PANELS_ROUTING_HPP
#define GUI_PANELS_ROUTING_HPP

#include <map>
#include <vector>

#include "../../song/links/key.hpp"
#include "../../song/links/type.hpp"
#include "../imgui/imgui.h"
#include "panel.hpp"

struct NodeInfo {
    int id;
    Target type;
    std::string name;
    ImVec2 position;
    ImVec2 size;
    std::map<LinkKey, ImVec2> input_pins;
    std::map<LinkKey, ImVec2> output_pins;
};

class GUIRoutingPanel : public GUIPanel {
  public:
    GUIRoutingPanel();

    void update() override;

  private:
    void add() override;
    void remove() override;
    void from() override;
    void to() const override;
    void draw() override;
    void check_keyboard_input() override;

    void collect_nodes();
    void draw_node(NodeInfo &node_info, ImDrawList *draw_list, const ImVec2 node_rect_min);
    void draw_input_pins(NodeInfo &node_info, ImDrawList *draw_list, const ImVec2 node_rect_min, const float node_title_height);
    void draw_output_pins(NodeInfo &node_info, ImDrawList *draw_list, const ImVec2 node_rect_max, const float node_title_height);
    void draw_all_links(ImDrawList *draw_list, const std::map<std::pair<Target, int>, NodeInfo *> &node_lookup);

    std::vector<NodeInfo> nodes;
    ImVec2 panel_scroll = {0.0f, 0.0f};
    const float node_width = 150.0f;
    const float pin_radius = 4.0f;
};

#endif // GUI_PANELS_ROUTING_HPP
