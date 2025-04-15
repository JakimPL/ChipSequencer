#ifndef GUI_PANELS_ROUTING_HPP
#define GUI_PANELS_ROUTING_HPP

#include <map>
#include <vector>

#include "../../song/links/key.hpp"
#include "panel.hpp"

struct NodeInfo {
    int id;
    ItemType type;
    std::string name;
    ImVec2 pos;
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

    void draw_node(NodeInfo &node_info, ImDrawList *draw_list);
    void draw_connections(ImDrawList *draw_list);
    void collect_nodes();

    std::vector<NodeInfo> nodes;
    std::map<LinkKey, ImVec2> all_input_pin_positions;
    std::map<LinkKey, ImVec2> all_output_pin_positions;
    ImVec2 panel_scroll = {0.0f, 0.0f};
    const float node_width = 150.0f;
    const float pin_radius = 4.0f;
};

#endif // GUI_PANELS_ROUTING_HPP
