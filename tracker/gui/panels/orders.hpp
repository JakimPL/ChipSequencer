#pragma once

#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>

#include "../constants.hpp"
#include "../init.hpp"
#include "../inputs/numeric.hpp"
#include "panel.hpp"

class GUIOrdersPanel : public GUIPanel {
  private:
    struct CurrentOrder {
        int length = 0;
        std::vector<int> sequences = {};
    } current_order;

    int selected_sequence = 0;
    NumericInputHandler input_handler;

    bool is_index_valid() const;
    void draw_order_length();
    void draw_order();

    bool select_item() override;
    void empty() override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIOrdersPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;

    void update() override;
    void set_index(const int index) override;
    void set_sequence(const size_t sequence_index, const size_t new_sequence);

    void from() override;
    void to() const override;

    int order_index = 0;
};
