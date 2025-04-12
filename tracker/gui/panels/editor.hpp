#ifndef GUI_PANELS_EDITOR_HPP
#define GUI_PANELS_EDITOR_HPP

#include <functional>

#include "../init.hpp"
#include "panel.hpp"

class GUIEditorPanel : public GUIPanel {
  private:
    int &current_octave;
    int &jump_step;
    int &page_size;

    void draw_panels();

    void from() override {};
    void to() const override {};
    void add() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIEditorPanel(
        int &current_octave,
        int &jump_step,
        int &page_size
    );

    void update() override {};
};

#endif // GUI_PANELS_EDITOR_HPP