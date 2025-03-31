#include <cstdio>
#include <iostream>

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
}

void GUIMenu::file_save() {
    song.save_to_file("temp.seq");
}

void GUIMenu::file_save_as() {
    song.save_to_file("temp.seq");
}

void GUIMenu::file_open() {
    song.load_from_file("temp.seq");
}

void GUIMenu::file_exit() {
    terminate();
}
