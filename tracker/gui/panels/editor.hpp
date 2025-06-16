#pragma once

#include <functional>

#include "../init.hpp"
#include "panel.hpp"

class GUIEditorPanel : public GUIPanel {
  private:
    const std::string label = "Editor";

    void draw_tabs();
    void draw_options();
    void draw_history();

    void draw() override;
    void check_keyboard_input() override;
};
