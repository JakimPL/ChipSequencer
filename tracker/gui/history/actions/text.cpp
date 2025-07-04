#include <sstream>

#include "../../../utils/string.hpp"
#include "../../constants.hpp"
#include "text.hpp"

template <size_t n>
ChangeTextAction<n>::ChangeTextAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    TextChange<n> txt_ch
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

template class ChangeTextAction<GUI_MAX_STRING_LENGTH>;
