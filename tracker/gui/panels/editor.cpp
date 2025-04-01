#include "editor.hpp"

GUIEditor::GUIEditor(int &octave)
    : current_octave(octave) {
}

void GUIEditor::draw() {
    ImGui::Begin("Editor");
    ImGui::Text("Current octave:");
    ImGui::SliderInt("##CurrentOctave", &current_octave, 0, 8);
    ImGui::End();
}
