#ifndef GUI_PANELS_ORDERS_HPP
#define GUI_PANELS_ORDERS_HPP

#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>

#include "../constants.hpp"
#include "../init.hpp"
#include "panel.hpp"

class GUIOrdersPanel : public GUIPanel {
  private:
    struct CurrentOrder {
        int length = 0;
        std::vector<int> sequences = {};
    } current_order;

    int order_index = 0;
    int selected_sequence = 0;
    const int max_items = GUI_MAX_ITEMS;
    std::string digit_buffer;

    bool is_index_valid() const;
    void draw_order_length();
    void draw_order();

    void from() override;
    void to() const override;
    void add() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIOrdersPanel();
    void update() override;
};

#endif // GUI_PANELS_ORDERS_HPP
