#include <sstream>

#include "../../names.hpp"
#include "../../panels/routings.hpp"
#include "routing.hpp"

ChangeRoutingAction::ChangeRoutingAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    const RoutingChange &rout_ch
)
    : Action(nm, own, k), routing_change(rout_ch) {
}

void ChangeRoutingAction::redo() {
    GUIRoutingsPanel *panel = dynamic_cast<GUIRoutingsPanel *>(owner);
    panel->set_link(routing_change.input_key, routing_change.new_routing);
}

void ChangeRoutingAction::undo() {
    GUIRoutingsPanel *panel = dynamic_cast<GUIRoutingsPanel *>(owner);
    panel->set_link(routing_change.input_key, routing_change.old_routing);
}

bool ChangeRoutingAction::can_merge(const Action *other) const {
    if (!Action::can_merge(other)) {
        return false;
    }
    return dynamic_cast<const ChangeRoutingAction *>(other) != nullptr;
}

void ChangeRoutingAction::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeRoutingAction *>(other);
    if (other_change) {
        routing_change.new_routing = other_change->routing_change.new_routing;
    }
}

std::string ChangeRoutingAction::get_name() const {
    std::ostringstream stream;
    const std::string old_value_target = target_names.at(routing_change.old_routing.target);
    const std::string new_value_target = target_names.at(routing_change.new_routing.target);
    stream << "Change routing for " << name
           << " from " << old_value_target << " " << routing_change.old_routing.index
           << " to " << new_value_target << " " << routing_change.new_routing.index;

    return stream.str();
}
