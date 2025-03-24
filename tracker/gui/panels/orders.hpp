#ifndef GUI_PANELS_ORDERS_HPP
#define GUI_PANELS_ORDERS_HPP

#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>
#include "../init.hpp"
#include "../names.hpp"
#include "utils.hpp"

class GUIOrdersPanel {
  private:
    struct CurrentOrder {
        int length;
        std::vector<int> sequences;
    } current_order;

    int order_index = 0;
    int selected_sequence = 0;
    const int max_items = 64;

    void from_order();
    void to_order();
    void update_orders();
    void draw_order_length();
    void draw_order();

  public:
    GUIOrdersPanel();
    void draw();
};

#endif // GUI_PANELS_ORDERS_HPP
