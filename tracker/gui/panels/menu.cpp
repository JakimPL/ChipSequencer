#include <cstdio>
#include <iostream>
#include "nfd/src/include/nfd.h"

#include "../../general.hpp"
#include "../../utils/file.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "menu.hpp"

GUIMenu::GUIMenu(const bool visible)
    : GUIPanel(visible) {
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileNew,
        {true, false, false, ImGuiKey_N},
        [this]() { file_new_confirm(); }
    );
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileOpen,
        {true, false, false, ImGuiKey_O},
        [this]() { file_open(); }
    );
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileSave,
        {true, false, false, ImGuiKey_S},
        [this]() { file_save(); }
    );
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileSaveAs,
        {true, false, true, ImGuiKey_S},
        [this]() { file_save_as(); }
    );
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileRender,
        {true, false, false, ImGuiKey_R},
        [this]() { file_render(); }
    );
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileCompileCompressed,
        {true, false, false, ImGuiKey_C},
        [this]() { file_compile(CompilationScheme::Compressed, CompilationTarget::Linux); }
    );
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileCompileUncompressed,
        {true, true, false, ImGuiKey_C},
        [this]() { file_compile(CompilationScheme::Uncompressed, CompilationTarget::Linux); }
    );
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileCompileDebug,
        {true, true, false, ImGuiKey_D},
        [this]() { file_compile(CompilationScheme::Debug, CompilationTarget::Linux); }
    );
    shortcut_manager.register_shortcut_and_action(
        ShortcutAction::FileExit,
        {true, false, false, ImGuiKey_Q},
        [this]() { file_exit_confirm(); }
    );
}

void GUIMenu::draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (get_menu_item("New", ShortcutAction::FileNew)) {
                file_new_confirm();
            }
            if (get_menu_item("Save", ShortcutAction::FileSave)) {
                file_save();
            }
            if (get_menu_item("Save as", ShortcutAction::FileSaveAs)) {
                file_save_as();
            }
            ImGui::Separator();
            if (get_menu_item("Open", ShortcutAction::FileOpen)) {
                file_open();
            }
            ImGui::Separator();
            if (get_menu_item("Render", ShortcutAction::FileRender)) {
                file_render();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Compile")) {
                if (get_menu_item("Compressed", ShortcutAction::FileCompileCompressed)) {
                    file_compile(CompilationScheme::Compressed, CompilationTarget::Linux);
                }
                if (get_menu_item("Uncompressed", ShortcutAction::FileCompileUncompressed)) {
                    file_compile(CompilationScheme::Uncompressed, CompilationTarget::Linux);
                }
                if (get_menu_item("Debug", ShortcutAction::FileCompileDebug)) {
                    file_compile(CompilationScheme::Debug, CompilationTarget::Linux);
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (get_menu_item("Exit", ShortcutAction::FileExit)) {
                file_exit_confirm();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            for (const auto &[element, name] : menu_items) {
                const bool visible = gui.get_visibility(element);
                if (get_menu_item(name, std::nullopt, visible)) {
                    gui.set_visibility(element, !visible);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

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

    if (open_exit_confirmation_popup) {
        ImGui::OpenPopup("Confirm exit");
        open_exit_confirmation_popup = false;
    }

    if (ImGui::BeginPopupModal("Compilation success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("File compiled successfully!");
    } else if (ImGui::BeginPopupModal("Compilation failure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Compilation failed!");
    } else if (ImGui::BeginPopupModal("Render success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Song rendered successfully!");
    } else if (ImGui::BeginPopupModal("Render failure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Song render failed!");
    } else if (ImGui::BeginPopupModal("Load error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Failed to load file!");
    } else if (ImGui::BeginPopupModal("Confirm new song", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        const std::string message = "Do you want to create a new song?\nAny unsaved changes will be lost.";
        draw_confirmation_popup(message, [this]() { file_new(); }, [this]() { file_save(); });
    } else if (ImGui::BeginPopupModal("Confirm exit", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        const std::string message = "Do you want to exit the program?\nAny unsaved changes will be lost.";
        draw_confirmation_popup(message, [this]() { file_exit(); }, [this]() { file_save(); });
    }
}

void GUIMenu::file_new_confirm() {
    open_new_song_confirmation_popup = true;
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
        song.save_to_file(current_path);
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
        gui.save(new_path);
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
            gui.open(file_path);
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
            song.render(wav_path);
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
    if (compilation_target != CompilationTarget::Linux) {
        std::cerr << "Unsupported compilation target!" << std::endl;
        return;
    }

    const std::string platform = "linux";
    nfdresult_t result = NFD_SaveDialog(platform == "linux" ? "" : "exe", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::filesystem::path new_path(target_path);
        new_path = check_and_correct_path_by_extension(new_path, platform == "linux" ? "" : ".exe");

        gui.stop();
        try {
            song.compile(new_path, scheme, compilation_target);
            compilation_status = std::filesystem::exists(new_path);
        } catch (std::runtime_error &exception) {
            compilation_status = false;
            std::cerr << "Compilation error: " << exception.what() << std::endl;
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
    open_exit_confirmation_popup = true;
}
