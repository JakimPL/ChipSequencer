#include <sstream>

#include "../../../general.hpp"
#include "lock.hpp"

ChangeLockAction::ChangeLockAction(
    const std::string &name,
    GUIPanel *owner,
    const LinkKey key,
    const bool value
) : Action(name, owner, key), new_value(value) {
}

void ChangeLockAction::redo() {
    if (new_value) {
        lock_registry.lock_item(key.target, key.index);
    } else {
        lock_registry.unlock_item(key.target, key.index);
    }
}

void ChangeLockAction::undo() {
    if (new_value) {
        lock_registry.unlock_item(key.target, key.index);
    } else {
        lock_registry.lock_item(key.target, key.index);
    }
}

bool ChangeLockAction::can_merge(const Action *other) const {
    return false;
}

void ChangeLockAction::merge(const Action *other) {
    return;
}

std::string ChangeLockAction::get_name() const {
    std::ostringstream stream;
    stream << (new_value ? "Locked " : "Unlocked ") << name;
    return stream.str();
}
