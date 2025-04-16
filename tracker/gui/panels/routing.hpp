#ifndef GUI_PANELS_ROUTING_HPP
#define GUI_PANELS_ROUTING_HPP

#include <map>
#include <string>
#include <vector>

#include "../../song/links/key.hpp"
#include "../../song/links/type.hpp"
#include "../imgui/imgui.h"
#include "panel.hpp"

class GUIRoutingPanel : public GUIPanel {
  private:
    struct NodeInfo {
        int id;
        Target type;
        std::string name;
        std::vector<std::string> parameters;
        ImVec2 position;
        ImVec2 size;
        int lines = 1;
    };

    std::vector<NodeInfo> nodes;
    std::map<std::pair<ItemType, int>, LinkKey> nodes_links;

    ImVec2 panel_scroll = {0.0f, 0.0f};
    const float node_width = 150.0f;
    const float pin_radius = 4.0f;

    void collect_links();
    void collect_nodes();
    void draw_node(NodeInfo &node_info, ImDrawList *draw_list, const ImVec2 node_rect_min);
    void draw_nodes();

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
