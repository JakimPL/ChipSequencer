#ifndef GUI_PANELS_ORDERS_HPP
#define GUI_PANELS_ORDERS_HPP

#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>
#include "../init.hpp"
#include "../names.hpp"
#include "panel.hpp"
#include "utils.hpp"

class GUIOrdersPanel : GUIPanel {
  private:
    struct CurrentOrder {
        int length = 0;
        std::vector<int> sequences = {};
    } current_order;

    int order_index = 0;
    int selected_sequence = 0;
    const int max_items = 64;

    bool is_index_valid() const;
    void draw_order_length();
    void draw_order();

    void from() override;
    void to() const override;
    void add() override;
    void remove() override;

  public:
    GUIOrdersPanel();
    void draw();
    void update();
};

#endif // GUI_PANELS_ORDERS_HPP
