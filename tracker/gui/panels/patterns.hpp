#ifndef GUI_PANELS_PATTERNS_HPP
#define GUI_PANELS_PATTERNS_HPP

#include <map>
#include <vector>

#include "../constants.hpp"
#include "../pattern.hpp"
#include "panel.hpp"

class GUIPatternsPanel : public GUIPanel {
  private:
    struct CurrentPattern {
        int page = 0;
        size_t total_rows = 0;
        std::map<size_t, std::vector<Pattern>> patterns;
        std::map<size_t, int> playing_rows;
    } current_pattern;

    size_t current_channel = -1;
    size_t current_index = -1;
    int current_row = -1;

    void draw_channels();
    void draw_channel(size_t channel_index);
    Pattern *find_pattern_by_current_row();

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
