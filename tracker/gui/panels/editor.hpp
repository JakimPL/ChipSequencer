#pragma once

#include <functional>

#include "../init.hpp"
#include "panel.hpp"

class GUIEditorPanel : public GUIPanel {
  private:
    int &current_octave;
    int &jump_step;
    int &page_size;

    void draw_panels();

    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIEditorPanel(
        const bool visible,
        int &current_octave,
        int &jump_step,
        int &page_size
    );
};
