#pragma once

#include <memory>

#include "../init.hpp"

class Action;

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

    void add_action(Action *action, const bool undo);

  protected:
    std::vector<std::pair<Action *, bool>> pending_actions;

    void actions();
    void draw_add_or_remove(
        const std::vector<std::string> &dependencies = {},
        const std::vector<std::pair<ItemType, uint8_t>> &link_dependencies = {}
    );

    virtual void add() = 0;
    virtual void duplicate() = 0;
    virtual void remove() = 0;
    virtual void draw() = 0;
    virtual void check_keyboard_input() = 0;
};
