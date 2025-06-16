#pragma once

#include <memory>

#include "../init.hpp"

class Action;

class GUIPanel {
  public:
    GUIPanel(const bool visible = true);
    virtual ~GUIPanel() = default;

    bool visible = true;

    virtual void update() {};
    virtual void set_index(const int) {};
    void frame();
    virtual void from() {};
    virtual void to() const {};

    void add_action(Action *action, const bool undo);

  protected:
    bool save = false;
    const std::string label;
    std::vector<std::pair<Action *, bool>> pending_actions;

    void history_actions();
    void draw_add_or_remove(
        const std::vector<std::string> &dependencies = {},
        const std::vector<std::pair<ItemType, uint8_t>> &link_dependencies = {}
    );

    virtual bool select_item() { return true; };
    virtual void empty() {};
    virtual void add() {};
    virtual void duplicate() {};
    virtual void remove() {};
    virtual void draw() {};
    virtual void check_keyboard_input() {};
    virtual void shortcut_actions() {};
    virtual void post_actions() { save = false; };
};
