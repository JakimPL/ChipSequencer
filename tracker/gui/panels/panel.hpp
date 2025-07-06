#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../song/links/target.hpp"
#include "../../song/links/type.hpp"
#include "../enums.hpp"

class Action;

class GUIPanel {
  public:
    explicit GUIPanel(
        std::string label,
        bool visible = true,
        bool windowed = true
    );
    virtual ~GUIPanel() = default;

    virtual GUIElement get_element() const = 0;
    virtual void update() {};
    virtual void set_index(int index) {};
    virtual void from() {};
    virtual void to() const {};
    void initialize();

    bool focus = false;
    bool visible = true;
    const bool windowed;

    void frame();
    void add_action(Action *action, bool undo);

    bool is_focused() const;
    void set_focus(bool focus = true);

  protected:
    bool save = false;
    const std::string label;
    std::vector<std::pair<Action *, bool>> pending_actions;

    virtual void register_shortcuts() {};
    virtual bool is_disabled() const { return false; };
    virtual bool select_item() { return true; };
    virtual void empty() {};
    virtual void add() {};
    virtual void duplicate() {};
    virtual void remove() {};
    virtual void draw() {};
    virtual void draw_dialog_box() {};
    virtual void check_keyboard_input() {};
    virtual void shortcut_actions() {};
    virtual void pre_actions() {};
    virtual void post_actions() { save = false; };

    void history_actions();
    void lock_item(Target target, size_t index);
    void draw_add_or_remove(
        const std::vector<std::string> &dependencies = {},
        const std::vector<std::pair<ItemType, uint8_t>> &link_dependencies = {}
    );
};
