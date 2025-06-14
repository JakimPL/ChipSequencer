#pragma once

#include <optional>

#include "panel.hpp"

class GUISummaryPanel : public GUIPanel {
  private:
    void draw_summary();
    void draw_table_row(bool highlight, const char *label, size_t size);
    void draw_table_row(bool highlight, const char *label, std::optional<size_t> count, size_t size);
    size_t draw_summary_components();
    size_t draw_summary_song_data();
    void draw_optimizations();

    void add() override {};
    void duplicate() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override {};

  public:
    GUISummaryPanel(const bool visible = true);
    void update() override {};
    void set_index(const int index) override {};

    void from() override {};
    void to() const override {};
};
