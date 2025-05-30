#ifndef GUI_PANELS_PANEL_HPP
#define GUI_PANELS_PANEL_HPP

#include "../init.hpp"

class GUIPanel {
  public:
    GUIPanel(const bool visible = true);
    virtual ~GUIPanel() = default;

    bool visible = true;

    virtual void update() = 0;
    virtual void set_index(const int index) = 0;
    void frame();
    virtual void from() = 0;
    virtual void to() const = 0;

  protected:
    void draw_add_or_remove(const std::string label = "", std::vector<size_t> dependencies = {});

    virtual void add() = 0;
    virtual void duplicate() = 0;
    virtual void remove() = 0;
    virtual void draw() = 0;
    virtual void check_keyboard_input() = 0;
};

#endif // GUI_PANELS_PANEL_HPP
