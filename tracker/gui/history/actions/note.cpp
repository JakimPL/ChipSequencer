#include <sstream>
#include <stdexcept>

#include "../../names.hpp"
#include "../../panels/patterns.hpp"
#include "../../panels/sequences.hpp"
#include "note.hpp"

ChangeNoteAction::ChangeNoteAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    const NoteChange &note_ch
)
    : Action(nm, own, k), note_change(note_ch) {
}

void ChangeNoteAction::redo() {
    GUIElement element = owner->get_element();
    if (element == GUIElement::Patterns) {
        GUIPatternsPanel *panel = static_cast<GUIPatternsPanel *>(owner);
        panel->set_note(note_change.pattern_row.channel_index, note_change.pattern_row.pattern_id, note_change.pattern_row.row, note_change.new_note);
    } else if (element == GUIElement::Sequences) {
        GUISequencesPanel *panel = static_cast<GUISequencesPanel *>(owner);
        panel->set_note(note_change.pattern_row.channel_index, note_change.pattern_row.row, note_change.new_note);
    } else {
        throw std::runtime_error("Invalid GUIElement for ChangeNoteAction");
    }
}

void ChangeNoteAction::undo() {
    GUIElement element = owner->get_element();
    if (element == GUIElement::Patterns) {
        GUIPatternsPanel *panel = static_cast<GUIPatternsPanel *>(owner);
        panel->set_note(note_change.pattern_row.channel_index, note_change.pattern_row.pattern_id, note_change.pattern_row.row, note_change.old_note);
    } else if (element == GUIElement::Sequences) {
        GUISequencesPanel *panel = static_cast<GUISequencesPanel *>(owner);
        panel->set_note(note_change.pattern_row.channel_index, note_change.pattern_row.row, note_change.old_note);
    } else {
        throw std::runtime_error("Invalid GUIElement for ChangeNoteAction");
    }
}

bool ChangeNoteAction::can_merge(const Action *other) const {
    const ChangeNoteAction *change_note_action = dynamic_cast<const ChangeNoteAction *>(other);
    if (change_note_action == nullptr) {
        return false;
    }

    return change_note_action->get_pattern_row() == note_change.pattern_row;
}

void ChangeNoteAction::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeNoteAction *>(other);
    if (other_change) {
        note_change.new_note = other_change->note_change.new_note;
    }
}

std::string ChangeNoteAction::get_name() const {
    std::ostringstream oss;
    oss << "Change " << name
        << " row " << note_change.pattern_row.row
        << " note " << get_full_note_name(note_change.old_note)
        << " to " << get_full_note_name(note_change.new_note);

    return oss.str();
}

PatternRow ChangeNoteAction::get_pattern_row() const {
    return note_change.pattern_row;
}
