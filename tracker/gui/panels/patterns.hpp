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

    const int page_size = GUI_PAGE_SIZE;

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
