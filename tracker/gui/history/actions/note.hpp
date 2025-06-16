#pragma once

#include "../changes/note.hpp"
#include "action.hpp"

class ChangeNoteAction : public Action {
  private:
    NoteChange note_change;

  public:
    ChangeNoteAction(
        const std::string &nm,
        GUIPanel *own,
        const LinkKey k,
        const NoteChange &note_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
