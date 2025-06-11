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
    void register_shortcut_and_action(
        const ShortcutAction id,
        const Shortcut &shortcut,
        const std::function<void()> &action
    );
    const Shortcut &get_shortcut(const ShortcutAction id) const;
    std::string get_shortcut_display(const ShortcutAction id) const;
    void process_shortcuts() const;

  private:
    std::unordered_map<ShortcutAction, Shortcut> shortcuts;
    std::unordered_map<ShortcutAction, std::function<void()>> actions;

    void register_action(const ShortcutAction id, const std::function<void()> &action);
    void register_shortcut(const ShortcutAction id, const Shortcut &shortcut);
};

#endif // GUI_SHORTCUTS_HPP
