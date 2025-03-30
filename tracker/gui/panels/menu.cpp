#include <cstdio>
#include "menu.hpp"
#include "imgui.h"

static void menu_new() {
    printf("New menu action executed.\n");
}

static void menu_save() {
    printf("Save menu action executed.\n");
}

static void menu_save_as() {
    printf("Save As menu action executed.\n");
}

static void menu_open() {
    printf("Open menu action executed.\n");
}

static void menu_exit() {
    printf("Open menu action executed.\n");
}

GUIMenu::GUIMenu()
    : callback_new(menu_new),
      callback_save(menu_save),
      callback_save_as(menu_save_as),
      callback_open(menu_open),
      callback_exit(menu_exit) {
}

void GUIMenu::draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Files")) {
            if (ImGui::MenuItem("New")) {
                if (callback_new) {
                    callback_new();
                }
            }
            if (ImGui::MenuItem("Save")) {
                if (callback_save) {
                    callback_save();
                }
            }
            if (ImGui::MenuItem("Save As")) {
                if (callback_save_as) {
                    callback_save_as();
                }
            }
            if (ImGui::MenuItem("Open")) {
                if (callback_open) {
                    callback_open();
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                if (callback_exit) {
                    callback_exit();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}
