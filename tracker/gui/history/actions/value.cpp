#include <iomanip>
#include <sstream>

#include "value.hpp"

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

template class ChangeValueAction<bool>;
template class ChangeValueAction<int>;
template class ChangeValueAction<float>;
