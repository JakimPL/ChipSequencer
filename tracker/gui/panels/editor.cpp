#include "../../general.hpp"
#include "../constants.hpp"
#include "editor.hpp"

GUIEditorPanel::GUIEditorPanel(int &octave, int &step)
    : current_octave(octave), jump_step(step) {
}

void GUIEditorPanel::draw() {
    check_keyboard_input();
    draw_panels();
}

void GUIEditorPanel::draw_panels() {
    const int min_octave = frequency_table.get_min_octave();
    const int max_octave = frequency_table.get_max_octave();
    ImGui::Begin("Editor");
    ImGui::Text("Current octave:");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##CurrentOctave", &current_octave, min_octave, max_octave);
    ImGui::Text("Jump step:");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##JumpStep", &jump_step, 0, GUI_MAX_JUMP_STEP);
    ImGui::End();
}

void GUIEditorPanel::check_keyboard_input() {
    if (ImGui::IsKeyPressed(ImGuiKey_KeypadMultiply)) {
        const int max_octave = frequency_table.get_max_octave();
        current_octave = std::min(current_octave + 1, max_octave);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_KeypadDivide)) {
        const int min_octave = frequency_table.get_min_octave();
        current_octave = std::max(current_octave - 1, min_octave);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_PageDown)) {
        jump_step = std::max(jump_step - 1, 0);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_PageUp)) {
        jump_step = std::min(jump_step + 1, GUI_MAX_JUMP_STEP);
    }
}
