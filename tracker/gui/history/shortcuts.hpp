#pragma once

#include <functional>
#include <string>
#include <unordered_map>

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

class ShortcutManager {
  public:
    ShortcutManager() = default;
    ~ShortcutManager() = default;
    ShortcutManager(const ShortcutManager &) = delete;
    ShortcutManager &operator=(const ShortcutManager &) = delete;
    ShortcutManager(ShortcutManager &&) = delete;
    ShortcutManager &operator=(ShortcutManager &&) = delete;

    void register_shortcut(
        const ShortcutAction id,
        const std::function<void()> &action
    );
    const Shortcut &get_shortcut(const ShortcutAction id) const;
    std::string get_shortcut_display(const ShortcutAction id) const;
    void process_shortcuts() const;
    void execute_action(const ShortcutAction id) const;
    bool is_shortcut_pressed(const ShortcutAction &action) const;
    bool is_shortcut_pressed(const Shortcut &shortcut) const;

  private:
    std::unordered_map<ShortcutAction, std::vector<std::function<void()>>> actions;
    const std::unordered_map<ShortcutAction, Shortcut> shortcuts = {
        {ShortcutAction::FileNew, {true, false, false, ImGuiKey_N}},
        {ShortcutAction::FileOpen, {true, false, false, ImGuiKey_O}},
        {ShortcutAction::FileSave, {true, false, false, ImGuiKey_S}},
        {ShortcutAction::FileSaveAs, {true, false, true, ImGuiKey_S}},
        {ShortcutAction::FileRender, {true, true, false, ImGuiKey_R}},
        {ShortcutAction::FileCompileCompressed, {true, false, false, ImGuiKey_E}},
        {ShortcutAction::FileCompileUncompressed, {true, true, false, ImGuiKey_E}},
        {ShortcutAction::FileCompileDebug, {true, false, true, ImGuiKey_E}},
        {ShortcutAction::FileExit, {false, false, true, ImGuiKey_F4}},
        {ShortcutAction::ApplicationFullscreen, {false, false, true, ImGuiKey_Enter}},
        {ShortcutAction::PlayerPlayPause, {false, false, false, ImGuiKey_Space}},
        {ShortcutAction::PlayerPlayFromCurrentPage, {true, false, false, ImGuiKey_Space}},
        {ShortcutAction::PlayerPlayFromCurrentPosition, {true, true, false, ImGuiKey_Space}},
        {ShortcutAction::PlayerFollowPlayback, {true, false, false, ImGuiKey_F}},
        {ShortcutAction::SongStop, {false, false, false, ImGuiKey_Escape}},
        {ShortcutAction::EditUndo, {true, false, false, ImGuiKey_Z}},
        {ShortcutAction::EditRedo, {true, true, false, ImGuiKey_Z}},
        {ShortcutAction::EditDelete, {true, false, false, ImGuiKey_Delete}},
        {ShortcutAction::EditCut, {true, false, false, ImGuiKey_X}},
        {ShortcutAction::EditCopy, {true, false, false, ImGuiKey_C}},
        {ShortcutAction::EditPaste, {true, false, false, ImGuiKey_V}},
        {ShortcutAction::EditIncrement, {false, false, false, ImGuiKey_KeypadAdd}},
        {ShortcutAction::EditDecrement, {false, false, false, ImGuiKey_KeypadSubtract}},
        {ShortcutAction::EditHome, {false, false, false, ImGuiKey_Home}},
        {ShortcutAction::EditEnd, {false, false, false, ImGuiKey_End}},
        {ShortcutAction::PatternTransposeUp, {true, false, false, ImGuiKey_KeypadAdd}},
        {ShortcutAction::PatternTransposeDown, {true, false, false, ImGuiKey_KeypadSubtract}},
        {ShortcutAction::PatternTransposeOctaveUp, {true, true, false, ImGuiKey_KeypadAdd}},
        {ShortcutAction::PatternTransposeOctaveDown, {true, true, false, ImGuiKey_KeypadSubtract}},
        {ShortcutAction::PatternSelectAll, {true, false, false, ImGuiKey_A}},
        {ShortcutAction::PatternSelectChannel, {true, false, false, ImGuiKey_P}},
        {ShortcutAction::PatternSelectNone, {true, true, false, ImGuiKey_A}},
        {ShortcutAction::PatternSetNoteRest, {true, false, false, ImGuiKey_Backspace}},
        {ShortcutAction::PatternSetNoteCut, {true, false, false, ImGuiKey_GraveAccent}},
        {ShortcutAction::PatternSetNoteOff, {true, false, false, ImGuiKey_Equal}},
        {ShortcutAction::SequenceNoteDelete, {false, false, false, ImGuiKey_Delete}},
        {ShortcutAction::SequenceSetNoteRest, {false, false, false, ImGuiKey_Backspace}},
        {ShortcutAction::SequenceSetNoteCut, {false, false, false, ImGuiKey_GraveAccent}},
        {ShortcutAction::SequenceSetNoteOff, {false, false, false, ImGuiKey_Equal}},
    };
};
