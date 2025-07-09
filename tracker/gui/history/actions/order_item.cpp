#include <sstream>

#include "../../panels/orders.hpp"
#include "order_item.hpp"

ChangeOrderItemAction::ChangeOrderItemAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    OrderItemChange seq_ch
)
    : Action(nm, own, k), sequence_change(seq_ch) {
}

void ChangeOrderItemAction::redo() {
    GUIOrdersPanel *panel = dynamic_cast<GUIOrdersPanel *>(owner);
    panel->set_sequence(sequence_change.sequence_index, sequence_change.new_sequence);
}

void ChangeOrderItemAction::undo() {
    GUIOrdersPanel *panel = dynamic_cast<GUIOrdersPanel *>(owner);
    panel->set_sequence(sequence_change.sequence_index, sequence_change.old_sequence);
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
        sequence_change.new_sequence = other_change->sequence_change.new_sequence;
    }
}

std::string ChangeOrderItemAction::get_name() const {
    std::ostringstream stream;
    stream << "Change Order " << key.index
           << " sequence " << sequence_change.sequence_index
           << " from " << sequence_change.old_sequence
           << " to " << sequence_change.new_sequence;
    return stream.str();
}
