#ifndef GUI_PANELS_PATTERNS_HPP
#define GUI_PANELS_PATTERNS_HPP

#include "panel.hpp"

class GUIPatternsPanel : public GUIPanel {
  private:
    struct CurrentPattern {
        int current_row = -1;
    } current_pattern;

    int page = 0;

    void draw_channels();
    void draw_channel(size_t channel_index);

    void from() override;
    void to() const override;
    void add() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIPatternsPanel();
    void update() override;
};

#endif // GUI_PANELS_PATTERNS_HPP
