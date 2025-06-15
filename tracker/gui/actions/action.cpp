#include "action.hpp"
#include "../panels/panel.hpp"

void Action::notify_panel(const bool undo) {
    owner->add_action(this, undo);
}

void Action::redo() {
    execute();
};
