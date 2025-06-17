
#include <iomanip>
#include <sstream>

#include "../../panels/panel.hpp"
#include "action.hpp"

Action::Action(const std::string &nm, GUIPanel *own, const LinkKey k)
    : name(nm), owner(own), key(k) {
}

void Action::execute() {
    redo();
}

void Action::redo() {
    execute();
}

void Action::notify_panel(const bool undo) {
    owner->add_action(this, undo);
}

bool Action::can_merge(const Action *other) const {
    return key == other->key && !key.is_null();
}

void Action::merge(const Action *other) {
}

std::string Action::get_name() const {
    return name;
}
