#include <sstream>

#include "../../panels/orders.hpp"
#include "order_item.hpp"

ChangeOrderItemAction::ChangeOrderItemAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    OrderItemChange oi_ch
)
    : Action(nm, own, k), order_item_change(oi_ch) {
}

void ChangeOrderItemAction::redo() {
    GUIOrdersPanel *panel = dynamic_cast<GUIOrdersPanel *>(owner);
    panel->set_item(order_item_change.sequence_index, order_item_change.new_sequence);
}

void ChangeOrderItemAction::undo() {
    GUIOrdersPanel *panel = dynamic_cast<GUIOrdersPanel *>(owner);
    panel->set_item(order_item_change.sequence_index, order_item_change.old_sequence);
}

bool ChangeOrderItemAction::can_merge(const Action *other) const {
    if (!Action::can_merge(other)) {
        return false;
    }
    return dynamic_cast<const ChangeOrderItemAction *>(other) != nullptr;
}

void ChangeOrderItemAction::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeOrderItemAction *>(other);
    if (other_change != nullptr) {
        order_item_change.new_sequence = other_change->order_item_change.new_sequence;
    }
}

std::string ChangeOrderItemAction::get_name() const {
    std::ostringstream stream;
    stream << "Change Order " << key.index
           << " sequence " << order_item_change.sequence_index
           << " from " << order_item_change.old_sequence
           << " to " << order_item_change.new_sequence;
    return stream.str();
}
