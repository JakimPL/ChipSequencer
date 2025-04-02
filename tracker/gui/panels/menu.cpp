#include <cstdio>
#include <iostream>
#include "nfd/src/include/nfd.h"

#include "../../general.hpp"
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
            if (ImGui::MenuItem("Compile")) {
                file_compile();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Open")) {
                file_open();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                file_exit();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GUIMenu::file_new() {
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
        if (new_path.extension() != ".seq") {
            new_path.replace_extension(".seq");
        }

        current_path = new_path;
        song.save_to_file(current_path, false);
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
        song.load_from_file(current_path);
        gui.update();
    } else if (result != NFD_CANCEL) {
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

void GUIMenu::file_compile() {
    file_save();
    if (!current_path.empty()) {
        song.save_to_file(current_path, true);
    }
}

void GUIMenu::file_exit() {
    terminate();
}
