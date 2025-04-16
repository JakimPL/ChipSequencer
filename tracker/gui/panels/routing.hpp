#ifndef GUI_PANELS_ROUTING_HPP
#define GUI_PANELS_ROUTING_HPP

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "../../song/links/key.hpp"
#include "../../song/links/type.hpp"
#include "../imgui/imgui.h"
#include "panel.hpp"

typedef std::pair<ItemType, int> InputKey;
typedef LinkKey OutputKey;

class GUIRoutingPanel : public GUIPanel {
  private:
    struct NodeInfo {
        size_t id;
        std::optional<InputKey> key;
        Target type;
        std::string name;
        std::vector<std::pair<OutputKey, std::string>> parameters;
        ImVec2 position;
        ImVec2 size;
        int lines = 1;
    };

    std::vector<NodeInfo> nodes;
    std::map<InputKey, OutputKey> nodes_links;
    std::map<OutputKey, ImVec2> input_pins;
    std::map<InputKey, ImVec2> output_pins;

    ImVec2 panel_scroll = {0.0f, 0.0f};

    void collect_links();
    void collect_nodes();
    void draw_node(NodeInfo &node_info, const ImVec2 node_rect_min);
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
