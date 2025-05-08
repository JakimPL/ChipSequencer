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
}

void GUIMenu::draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New")) {
                file_new();
            }
            if (ImGui::MenuItem("Save")) {
                file_save();
            }
            if (ImGui::MenuItem("Save as")) {
                file_save_as();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open")) {
                file_open();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Render")) {
                file_render();
            }
            ImGui::Separator();
            if (ImGui::BeginMenu("Compile")) {
                if (ImGui::MenuItem("DOS")) {
                    file_compile(true, CompilationTarget::DOS);
                }
                if (ImGui::MenuItem("Linux")) {
                    file_compile(true, CompilationTarget::Linux);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Compile uncompressed")) {
                if (ImGui::MenuItem("DOS")) {
                    file_compile(false, CompilationTarget::DOS);
                }
                if (ImGui::MenuItem("Linux")) {
                    file_compile(false, CompilationTarget::Linux);
                }
                ImGui::EndMenu();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                file_exit();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            for (const auto &[element, name] : menu_items) {
                const bool visible = gui.get_visibility(element);
                if (ImGui::MenuItem(name, nullptr, visible)) {
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
    }
}

void GUIMenu::file_new() {
    gui.stop();
    song.new_song();
    current_path = std::filesystem::path();
    gui.update();
}

void GUIMenu::file_save() {
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

void GUIMenu::file_compile(const bool compress, const CompilationTarget compilation_target) {
    nfdchar_t *target_path = nullptr;
    const std::string platform = (compilation_target == CompilationTarget::DOS) ? "dos" : "linux";
    nfdresult_t result = NFD_SaveDialog(platform == "linux" ? "" : "exe", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::filesystem::path new_path(target_path);
        new_path = check_and_correct_path_by_extension(new_path, platform == "linux" ? "" : ".exe");

        gui.stop();
        try {
            song.compile(new_path, compress, compilation_target);
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
