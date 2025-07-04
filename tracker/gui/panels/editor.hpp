#pragma once

#include <functional>

#include "panel.hpp"

class GUIEditorPanel : public GUIPanel {
  private:
    void draw_tabs();
    void draw_options();
    static void draw_history();
    void draw_clipboard();

    void draw() override;
    void check_keyboard_input() override;

  public:
    explicit GUIEditorPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;
};
