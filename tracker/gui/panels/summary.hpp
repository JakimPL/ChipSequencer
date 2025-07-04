#pragma once

#include <optional>

#include "panel.hpp"

class GUISummaryPanel : public GUIPanel {
  private:
    void draw_summary();
    static void draw_table_row(bool highlight, const char *label, size_t size);
    static void draw_table_row(bool highlight, const char *label, std::optional<size_t> count, size_t size);
    size_t draw_summary_components();
    size_t draw_summary_song_data();
    static void draw_optimizations();

    void draw() override;

  public:
    explicit GUISummaryPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;
};
