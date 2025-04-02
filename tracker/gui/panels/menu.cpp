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

    if (ImGui::BeginPopupModal("Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("File compiled successfully!");
        float buttonWidth = 60.0f;
        float windowWidth = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
        if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    if (ImGui::BeginPopupModal("Failure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Compilation failed!");
        float buttonWidth = 60.0f;
        float windowWidth = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
        if (ImGui::Button("Close", ImVec2(buttonWidth, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void GUIMenu::file_new() {
    gui.stop();
    song.new_song();
    current_path = std::filesystem::path();
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
        song.load_from_file(current_path);
        gui.update();
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
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
