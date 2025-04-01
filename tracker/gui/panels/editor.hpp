#ifndef GUI_PANELS_EDITOR_HPP
#define GUI_PANELS_EDITOR_HPP

#include <functional>

#include "../init.hpp"

class GUIEditor {
  public:
    GUIEditor(int &current_octave);

    void draw();
    int &current_octave;
};

#endif // GUI_PANELS_EDITOR_HPP