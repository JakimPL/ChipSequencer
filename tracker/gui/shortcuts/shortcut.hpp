#pragma once

#include <string>

#include "../init.hpp"

enum class ShortcutAction {
    FileNew,
    FileOpen,
    FileSave,
    FileSaveAs,
    FileRender,
    FileCompileCompressed,
    FileCompileUncompressed,
    FileCompileDebug,
    FileExit,
    ApplicationFullscreen,
    PlayerPlayPause,
    PlayerPlayFromCurrentPage,
    PlayerPlayFromCurrentPosition,
    PlayerFollowPlayback,
    SongStop,
    EditUndo,
    EditRedo,
    EditDelete,
    EditCut,
    EditCopy,
    EditPaste,
    EditIncrement,
    EditDecrement,
    EditHome,
    EditEnd,
    PatternTransposeUp,
    PatternTransposeDown,
    PatternTransposeOctaveUp,
    PatternTransposeOctaveDown,
    PatternSelectAll,
    PatternSelectChannel,
    PatternSelectNone,
    PatternSetNoteRest,
    PatternSetNoteCut,
    PatternSetNoteOff,
    SequenceNoteDelete,
    SequenceSetNoteRest,
    SequenceSetNoteCut,
    SequenceSetNoteOff,
};

struct Shortcut {
    bool ctrl;
    bool shift;
    bool alt;
    ImGuiKey key;

    bool operator==(const Shortcut &other) const {
        return ctrl == other.ctrl && shift == other.shift &&
               alt == other.alt && key == other.key;
    }

    std::string get_display_string() const;
};
