#include <cstdio>
#include <iostream>
#include "nfd/src/include/nfd.h"

#include "../../general.hpp"
#include "../../utils/file.hpp"
#include "menu.hpp"

GUIMenu::GUIMenu() {
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
            if (ImGui::MenuItem("Save As")) {
                file_save_as();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open")) {
                file_open();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Compile")) {
                file_compile(true);
            }
            if (ImGui::MenuItem("Compile uncompressed")) {
                file_compile(false);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                file_exit();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (compilation_status.has_value()) {
        ImGui::OpenPopup(compilation_status.value() ? "Success" : "Failure");
        compilation_status = std::nullopt;
    }

    if (load_error.has_value()) {
        ImGui::OpenPopup("Load error");
        load_error = std::nullopt;
    }

    if (ImGui::BeginPopupModal("Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("File compiled successfully!");
    } else if (ImGui::BeginPopupModal("Failure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Compilation failed!");
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

        current_path = new_path;
        song.save_to_file(current_path);
        gui.change_window_title(current_path.filename().string());
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
        current_path = file_path;

        gui.stop();

        try {
            song.load_from_file(current_path);
        } catch (nlohmann::json::exception &e) {
            load_error = true;
            std::cerr << "Failed to parse JSON file: " << e.what() << std::endl;
        }

        gui.change_window_title(current_path.filename().string());
        gui.update();
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
    gui.update();
}

void GUIMenu::file_compile(const bool compress) {
    nfdchar_t *target_path = nullptr;
    nfdresult_t result = NFD_SaveDialog("exe", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::filesystem::path new_path(target_path);
        new_path = check_and_correct_path_by_extension(new_path, ".exe");

        gui.stop();
        song.compile(new_path, compress);
        compilation_status = std::filesystem::exists(new_path);
        gui.update();
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

void GUIMenu::file_exit() {
    gui.stop();
    terminate();
}
