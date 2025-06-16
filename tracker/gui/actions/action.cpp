
#include <iomanip>
#include <sstream>

#include "../../utils/string.hpp"
#include "../constants.hpp"
#include "../names.hpp"
#include "../panels/orders.hpp"
#include "../panels/panel.hpp"
#include "../panels/routings.hpp"
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

template <typename T>
ChangeValueAction<T>::ChangeValueAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    const ValueChange<T> &val_ch
)
    : Action(nm, own, k), value_change(val_ch) {
}

template <typename T>
void ChangeValueAction<T>::redo() {
    value_change.parameter = value_change.new_value;
}

template <typename T>
void ChangeValueAction<T>::undo() {
    value_change.parameter = value_change.old_value;
}

template <typename T>
bool ChangeValueAction<T>::can_merge(const Action *other) const {
    if (!Action::can_merge(other)) {
        return false;
    }
    return dynamic_cast<const ChangeValueAction<T> *>(other) != nullptr;
}

template <typename T>
void ChangeValueAction<T>::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeValueAction<T> *>(other);
    if (other_change) {
        value_change.new_value = other_change->value_change.new_value;
    }
}

template <typename T>
std::string ChangeValueAction<T>::get_name() const {
    std::ostringstream stream;
    stream << "Change " << name;
    if constexpr (std::is_floating_point_v<T>) {
        stream << " from " << std::setprecision(4) << value_change.old_value
               << " to " << std::setprecision(4) << value_change.new_value;
    } else {
        stream << " from " << value_change.old_value
               << " to " << value_change.new_value;
    }
    return stream.str();
}

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

ChangeOrderSequenceAction::ChangeOrderSequenceAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    const OrderSequenceChange &seq_ch
)
    : Action(nm, own, k), sequence_change(seq_ch) {
}

void ChangeOrderSequenceAction::redo() {
    GUIOrdersPanel *panel = dynamic_cast<GUIOrdersPanel *>(owner);
    panel->set_sequence(sequence_change.sequence_index, sequence_change.new_sequence);
}

void ChangeOrderSequenceAction::undo() {
    GUIOrdersPanel *panel = dynamic_cast<GUIOrdersPanel *>(owner);
    panel->set_sequence(sequence_change.sequence_index, sequence_change.old_sequence);
}

bool ChangeOrderSequenceAction::can_merge(const Action *other) const {
    if (!Action::can_merge(other)) {
        return false;
    }
    return dynamic_cast<const ChangeOrderSequenceAction *>(other) != nullptr;
}

void ChangeOrderSequenceAction::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeOrderSequenceAction *>(other);
    if (other_change) {
        sequence_change.new_sequence = other_change->sequence_change.new_sequence;
    }
}

std::string ChangeOrderSequenceAction::get_name() const {
    std::ostringstream stream;
    stream << "Change Order " << key.index
           << " sequence " << sequence_change.sequence_index
           << " from " << sequence_change.old_sequence
           << " to " << sequence_change.new_sequence;
    return stream.str();
}

template <size_t n>
ChangeTextAction<n>::ChangeTextAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    const TextChange<n> &txt_ch
)
    : Action(nm, own, k), text_change(txt_ch) {
}

template <size_t n>
void ChangeTextAction<n>::redo() {
    copy_string_to_buffer(text_change.new_value.c_str(), text_change.buffer, n);
}

template <size_t n>
void ChangeTextAction<n>::undo() {
    copy_string_to_buffer(text_change.old_value.c_str(), text_change.buffer, n);
}

template <size_t n>
bool ChangeTextAction<n>::can_merge(const Action *other) const {
    if (!Action::can_merge(other)) {
        return false;
    }
    return dynamic_cast<const ChangeTextAction<n> *>(other) != nullptr;
}

template <size_t n>
void ChangeTextAction<n>::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeTextAction<n> *>(other);
    if (other_change) {
        text_change.new_value = other_change->text_change.new_value;
    }
}

template <size_t n>
std::string ChangeTextAction<n>::get_name() const {
    std::ostringstream stream;
    stream << "Change " << name
           << " from \"" << text_change.old_value
           << "\" to \"" << text_change.new_value << "\"";
    return stream.str();
}

template class ChangeValueAction<float>;
template class ChangeValueAction<int>;
template class ChangeValueAction<bool>;
template class ChangeTextAction<GUI_MAX_STRING_LENGTH>;
