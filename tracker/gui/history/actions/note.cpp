#include "note.hpp"

ChangeNoteAction::ChangeNoteAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    const NoteChange &note_ch
)
    : Action(nm, own, k), note_change(note_ch) {
}
