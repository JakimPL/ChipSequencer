#ifndef GUI_SHORTCUTS_HPP
#define GUI_SHORTCUTS_HPP

#include <functional>
#include <string>
#include <unordered_map>

#include "init.hpp"

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
    SongPlayPause,
    SongStop,
    EditUndo,
    EditRedo,
    PatternTransposeUp,
    PatternTransposeDown,
    PatternTransposeOctaveUp,
    PatternTransposeOctaveDown,
    PatternSelectAll,
    PatternSelectNone,
    PatternClear,
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
    void register_shortcut(
        const ShortcutAction id,
        const std::function<void()> &action
    );
    const Shortcut &get_shortcut(const ShortcutAction id) const;
    std::string get_shortcut_display(const ShortcutAction id) const;
    void process_shortcuts() const;
    void execute_action(const ShortcutAction id) const;

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
        {ShortcutAction::SongPlayPause, {false, false, false, ImGuiKey_Space}},
        {ShortcutAction::SongStop, {false, false, false, ImGuiKey_Escape}},
        {ShortcutAction::EditUndo, {true, false, false, ImGuiKey_Z}},
        {ShortcutAction::EditRedo, {true, false, true, ImGuiKey_Z}},
        {ShortcutAction::PatternTransposeUp, {true, false, false, ImGuiKey_KeypadAdd}},
        {ShortcutAction::PatternTransposeDown, {true, false, false, ImGuiKey_KeypadSubtract}},
        {ShortcutAction::PatternTransposeOctaveUp, {true, true, false, ImGuiKey_KeypadAdd}},
        {ShortcutAction::PatternTransposeOctaveDown, {true, true, false, ImGuiKey_KeypadSubtract}},
        {ShortcutAction::PatternSelectAll, {true, false, false, ImGuiKey_A}},
        {ShortcutAction::PatternSelectNone, {true, true, false, ImGuiKey_A}},
        {ShortcutAction::PatternClear, {true, false, false, ImGuiKey_Delete}},
    };
};

#endif // GUI_SHORTCUTS_HPP
