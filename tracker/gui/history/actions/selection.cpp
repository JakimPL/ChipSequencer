#include <sstream>
#include <utility>

#include "../../patterns/commands.hpp"
#include "selection.hpp"

template <typename T>
ChangePatternSelectionAction<T>::ChangePatternSelectionAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    PatternSelectionChange<T> s_ch,
    SetItemsFunction<T> set_items_function
)
    : Action(nm, own, k), selection_changes(std::move(s_ch)), set_items(std::move(set_items_function)) {
}

template <typename T>
void ChangePatternSelectionAction<T>::redo() {
    std::map<PatternRow, T> changes;
    for (const auto &[pattern_row, change] : selection_changes) {
        changes[pattern_row] = change.second;
    }

    set_items(changes);
}

template <typename T>
void ChangePatternSelectionAction<T>::undo() {
    std::map<PatternRow, T> changes;
    for (const auto &[pattern_row, change] : selection_changes) {
        changes[pattern_row] = change.first;
    }

    set_items(changes);
}

template <typename T>
bool ChangePatternSelectionAction<T>::can_merge(const Action * /* other */) const {
    return false;
}

template <typename T>
void ChangePatternSelectionAction<T>::merge(const Action * /* other */) {
}

template <typename T>
std::string ChangePatternSelectionAction<T>::get_name() const {
    std::ostringstream stream;
    stream << name << " selection";
    return stream.str();
}

template class ChangePatternSelectionAction<uint8_t>;
template class ChangePatternSelectionAction<CommandValue>;
