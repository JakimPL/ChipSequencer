#include <sstream>

#include "selection.hpp"

void ChangePatternSelectionAction::redo() {
    std::map<PatternRow, uint8_t> changes;
    for (const auto &[pattern_row, change] : selection_change.changes) {
        changes[pattern_row] = change.second;
    }

    set_items(changes);
}

void ChangePatternSelectionAction::undo() {
    std::map<PatternRow, uint8_t> changes;
    for (const auto &[pattern_row, change] : selection_change.changes) {
        changes[pattern_row] = change.first;
    }

    set_items(changes);
}

bool ChangePatternSelectionAction::can_merge(const Action *other) const {
    return false;
}

void ChangePatternSelectionAction::merge(const Action *other) {
    return;
}

std::string ChangePatternSelectionAction::get_name() const {
    std::ostringstream stream;
    stream << name;
    return stream.str();
}
