#include <sstream>
#include <utility>

#include "../../../general.hpp"
#include "../../names.hpp"
#include "add.hpp"

AddItemAction::AddItemAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    AddFunction add,
    RemoveFunction remove
)
    : Action(nm, own, k),
      add_function(std::move(add)),
      remove_function(std::move(remove)) {
}

void AddItemAction::redo() {
    if (add_function) {
        add_function();
    }
}

void AddItemAction::undo() {
    if (remove_function) {
        remove_function(key.index);
    }
}

bool AddItemAction::can_merge(const Action * /*other */) const {
    return false;
}

void AddItemAction::merge(const Action * /* other */) {
}

std::string AddItemAction::get_name() const {
    std::ostringstream stream;
    stream << "Add " << name;
    return stream.str();
}
