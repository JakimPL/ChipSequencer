#ifndef GUI_PANELS_PANEL_HPP
#define GUI_PANELS_PANEL_HPP

#include "../init.hpp"

class GUIPanel {
  public:
    GUIPanel() = default;
    virtual ~GUIPanel() = default;

    virtual void draw() = 0;
    virtual void update() = 0;

  protected:
    void draw_add_or_remove();
    virtual void add() = 0;
    virtual void remove() = 0;
    virtual void from() = 0;
    virtual void to() const = 0;
};

#endif // GUI_PANELS_PANEL_HPP
