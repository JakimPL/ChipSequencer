#ifndef GUI_PANELS_SUMMARY_HPP
#define GUI_PANELS_SUMMARY_HPP

#include "panel.hpp"

class GUISummaryPanel : public GUIPanel {
  private:
    void draw_summary();
    void draw_optimizations();

    void add() override {};
    void duplicate() override {};
    void remove() override {};
    void from() override {};
    void to() const override {};
    void draw() override;
    void check_keyboard_input() override {};

  public:
    GUISummaryPanel(const bool visible = true);

    void update() override {};
    void set_index(const int index) override {};
};

#endif // GUI_PANELS_SUMMARY_HPP
