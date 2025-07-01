#pragma once

#include <functional>

#include "panel.hpp"

class GUIEditorPanel : public GUIPanel {
  private:
    void draw_tabs();
    void draw_options();
    void draw_history();

    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIEditorPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;
};
