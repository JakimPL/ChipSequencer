#include "../../general.hpp"
#include "../constants.hpp"
#include "editor.hpp"

GUIEditorPanel::GUIEditorPanel(const bool visible, int &octave, int &step, int &page)
    : GUIPanel(visible), current_octave(octave), jump_step(step), page_size(page) {
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

    const int previous_page_size = page_size;
    ImGui::Text("Page size:");
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SliderInt("##PageSize", &page_size, GUI_MIN_PAGE_SIZE, GUI_MAX_PAGE_SIZE);
    if (page_size != previous_page_size) {
        gui.deselect_all_rows();
    }

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
}
