#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "nfd.h"

#include "../../general.hpp"
#include "../../song/song.hpp"
#include "../../utils/paths.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "../history/manager.hpp"
#include "menu.hpp"

GUIMenu::GUIMenu(const bool visible, const bool windowed)
    : GUIPanel("Menu", visible, windowed) {
}

GUIElement GUIMenu::get_element() const {
    return GUIElement::Menu;
}

void GUIMenu::draw() {
    draw_menu();
}

void GUIMenu::draw_menu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            draw_menu_item("New", ShortcutAction::FileNew);
            draw_menu_item("Save", ShortcutAction::FileSave);
            draw_menu_item("Save as", ShortcutAction::FileSaveAs);
            ImGui::Separator();
            draw_menu_item("Open", ShortcutAction::FileOpen);
            ImGui::Separator();
            draw_menu_item("Render", ShortcutAction::FileRender);
            ImGui::Separator();
            if (ImGui::BeginMenu("Compile")) {
                draw_menu_item("Compressed", ShortcutAction::FileCompileCompressed);
                draw_menu_item("Uncompressed", ShortcutAction::FileCompileUncompressed);
                draw_menu_item("Debug", ShortcutAction::FileCompileDebug);
                ImGui::EndMenu();
            }
            ImGui::Separator();
            draw_menu_item("Exit", ShortcutAction::FileExit);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Player")) {
            const std::string play_pause_label = gui.is_playing() ? "Pause" : "Play";
            draw_menu_item(play_pause_label, ShortcutAction::PlayerPlayPause);
            draw_menu_item("Play from current page", ShortcutAction::PlayerPlayFromCurrentPage);
            ImGui::BeginDisabled(gui.get_current_row() < 0);
            draw_menu_item("Play from current position", ShortcutAction::PlayerPlayFromCurrentPosition);
            ImGui::EndDisabled();
            draw_menu_item("Stop", ShortcutAction::SongStop);
            ImGui::Separator();
            draw_menu_item("Follow playback", ShortcutAction::PlayerFollowPlayback, gui.follow_playback);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            draw_menu_item("Undo", ShortcutAction::EditUndo);
            draw_menu_item("Redo", ShortcutAction::EditRedo);
            ImGui::Separator();
            draw_menu_item("Delete", ShortcutAction::EditDelete);
            draw_menu_item("Cut", ShortcutAction::EditCut);
            draw_menu_item("Copy", ShortcutAction::EditCopy);
            draw_menu_item("Paste", ShortcutAction::EditPaste);
            if (gui.is_pattern_view_active()) {
                ImGui::Separator();
                draw_menu_item("Select all", ShortcutAction::PatternSelectAll);
                draw_menu_item("Select channel", ShortcutAction::PatternSelectChannel);
                draw_menu_item("Deselect all", ShortcutAction::PatternSelectNone);
                if (!gui.is_commands_pattern_view_active()) {
                    ImGui::Separator();
                    draw_menu_item("Transpose up", ShortcutAction::PatternTransposeUp);
                    draw_menu_item("Transpose down", ShortcutAction::PatternTransposeDown);
                    draw_menu_item("Transpose octave up", ShortcutAction::PatternTransposeOctaveUp);
                    draw_menu_item("Transpose octave down", ShortcutAction::PatternTransposeOctaveDown);
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            draw_menu_item("Fullscreen", ShortcutAction::ApplicationFullscreen, gui.is_fullscreen());
            ImGui::Separator();
            for (const auto &[element, name] : menu_items) {
                const bool visible = gui.get_visibility(element);
                draw_menu_item(name, std::nullopt, visible);
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GUIMenu::file_new_confirm() {
    if (history_manager.empty()) {
        file_new();
        return;
    }

    open_new_song_confirmation_popup = true;
}

void GUIMenu::file_open_confirm() {
    if (history_manager.empty()) {
        file_open();
        return;
    }

    open_open_song_confirmation_popup = true;
}

void GUIMenu::file_new() {
    gui.stop();
    gui.new_song();
    current_path = std::filesystem::path();
    gui.update();
    gui.change_window_title();
}

void GUIMenu::file_save() {
    gui.stop();
    if (current_path.empty()) {
        file_save_as();
    } else {
        song.save_to_file(current_path.string());
    }
}

void GUIMenu::file_save_as() {
    nfdchar_t *target_path = nullptr;
    nfdresult_t result = NFD_SaveDialog("seq", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::filesystem::path new_path(target_path);
        free(target_path);
        new_path = check_and_correct_path_by_extension(new_path, ".seq");
        gui.stop();
        gui.save(new_path.string());
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

void GUIMenu::file_open() {
    nfdchar_t *target_path = nullptr;
    nfdresult_t result = NFD_OpenDialog("seq", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::filesystem::path file_path(target_path);
        free(target_path);

        gui.stop();
        try {
            gui.open(file_path.string());
        } catch (nlohmann::json::exception &exception) {
            load_error = true;
            std::cerr << "Failed to parse JSON file: " << exception.what() << std::endl;
        } catch (const std::runtime_error &exception) {
            song.new_song();
            load_error = true;
            std::cerr << "Failed to read data file: " << exception.what() << std::endl;
        }
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
    gui.update();
}

void GUIMenu::file_render() {
    nfdchar_t *target_path = nullptr;
    nfdresult_t result = NFD_SaveDialog("wav", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::filesystem::path wav_path(target_path);
        free(target_path);
        wav_path = check_and_correct_path_by_extension(wav_path, ".wav");

        gui.stop();
        try {
            song.render(wav_path.string());
            render_status = std::filesystem::exists(wav_path);
        } catch (const std::exception &exception) {
            render_status = false;
            std::cerr << "Render error: " << exception.what() << std::endl;
        }
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

void GUIMenu::file_compile(const CompilationScheme scheme, const CompilationTarget compilation_target) {
    nfdchar_t *target_path = nullptr;

    const std::string platform = compilation_target == CompilationTarget::Linux ? "linux" : "windows";
    nfdresult_t result = NFD_SaveDialog(platform == "linux" ? "" : "exe", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::filesystem::path new_path(target_path);
        new_path = check_and_correct_path_by_extension(new_path, platform == "linux" ? "" : ".exe");

        gui.stop();
        try {
            song.compile(new_path.string(), scheme, compilation_target);
            compilation_status = std::filesystem::exists(new_path);
        } catch (std::runtime_error &exception) {
            compilation_status = false;
            compilation_error = exception.what();
            std::cerr << "Compilation error: " << compilation_error << std::endl;
        }
        gui.update();
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

void GUIMenu::file_exit() {
    gui.stop();
    terminate();
}

void GUIMenu::file_exit_confirm() {
    if (history_manager.empty()) {
        file_exit();
        return;
    }

    open_exit_confirmation_popup = true;
}

void GUIMenu::draw_dialog_box() {
    if (compilation_status.has_value()) {
        ImGui::OpenPopup(compilation_status.value() ? "Compilation success" : "Compilation failure");
        compilation_status = std::nullopt;
    }

    if (render_status.has_value()) {
        ImGui::OpenPopup(render_status.value() ? "Render success" : "Render failure");
        render_status = std::nullopt;
    }

    if (load_error.has_value()) {
        ImGui::OpenPopup("Load error");
        load_error = std::nullopt;
    }

    if (open_new_song_confirmation_popup) {
        ImGui::OpenPopup("Confirm new song");
        open_new_song_confirmation_popup = false;
    }

    if (open_open_song_confirmation_popup) {
        ImGui::OpenPopup("Confirm open song");
        open_open_song_confirmation_popup = false;
    }

    if (open_exit_confirmation_popup) {
        ImGui::OpenPopup("Confirm exit");
        open_exit_confirmation_popup = false;
    }

    if (ImGui::BeginPopupModal("Compilation success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("File compiled successfully!");
    } else if (ImGui::BeginPopupModal("Compilation failure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Compilation failed!\n\n" + compilation_error);
    } else if (ImGui::BeginPopupModal("Render success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Song rendered successfully!");
    } else if (ImGui::BeginPopupModal("Render failure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Song render failed!");
    } else if (ImGui::BeginPopupModal("Load error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Failed to load file!");
    } else if (ImGui::BeginPopupModal("Confirm new song", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        const std::string message = "Do you want to create a new song?\nAny unsaved changes will be lost.";
        draw_confirmation_popup(message, [this]() { file_new(); }, [this]() { file_save(); });
    } else if (ImGui::BeginPopupModal("Confirm open song", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        const std::string message = "Do you want to open a new song?\nAny unsaved changes will be lost.";
        draw_confirmation_popup(message, [this]() { file_open(); }, [this]() { file_save(); });
    } else if (ImGui::BeginPopupModal("Confirm exit", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        const std::string message = "Do you want to exit the program?\nAny unsaved changes will be lost.";
        draw_confirmation_popup(message, [this]() { file_exit(); }, [this]() { file_save(); });
    }
}

void GUIMenu::register_shortcuts() {
    shortcut_manager.register_shortcut(
        ShortcutAction::FileNew,
        [this]() { file_new_confirm(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::FileOpen,
        [this]() { file_open_confirm(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::FileSave,
        [this]() { file_save(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::FileSaveAs,
        [this]() { file_save_as(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::FileRender,
        [this]() { file_render(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::FileCompileCompressed,
        [this]() { file_compile(CompilationScheme::Compressed, DefaultCompilationTarget); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::FileCompileUncompressed,
        [this]() { file_compile(CompilationScheme::Uncompressed, DefaultCompilationTarget); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::FileCompileDebug,
        [this]() { file_compile(CompilationScheme::Debug, DefaultCompilationTarget); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::FileExit,
        [this]() { file_exit_confirm(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::ApplicationFullscreen,
        [this]() { gui.toggle_fullscreen(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditUndo,
        []() { history_manager.undo(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditRedo,
        []() { history_manager.redo(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PlayerFollowPlayback,
        [this]() { gui.follow_playback = !gui.follow_playback; }
    );
}
