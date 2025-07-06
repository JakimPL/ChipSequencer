#pragma once

#include <functional>

#include "panel.hpp"

class GUIEditorPanel : public GUIPanel {
  private:
    void draw_tabs();
    void draw_options();
    static void draw_history();
    static void draw_clipboard();
    static void draw_theme();
    static void save_theme();
    static void load_theme();

    void draw() override;
    void check_keyboard_input() override;

  public:
    explicit GUIEditorPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;
};
