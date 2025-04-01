#include "general.hpp"

void GUIGeneralPanel::from() {
    current_song.bpm = bpm;
    current_song.normalizer = normalizer;
}

void GUIGeneralPanel::to() const {
    bpm = current_song.bpm;
    normalizer = current_song.normalizer;
    calculate_ticks_per_beat();
}

void GUIGeneralPanel::draw_play_button() {
    ImVec2 p = ImGui::GetCursorScreenPos();
    float sz = 20.0f;
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 center = ImVec2(p.x + sz * 0.5f, p.y + sz * 0.5f);

    ImVec2 points[3] = {
        ImVec2(center.x - sz * 0.3f, center.y - sz * 0.5f),
        ImVec2(center.x + sz * 0.5f, center.y),
        ImVec2(center.x - sz * 0.3f, center.y + sz * 0.5f)
    };

    draw_list->AddTriangleFilled(points[0], points[1], points[2], IM_COL32(0, 255, 0, 255));

    ImGui::SetCursorScreenPos(p);
    ImGui::InvisibleButton("PlayButton", ImVec2(sz, sz));
    const bool clicked = ImGui::IsItemClicked();

    if (clicked && !is_playing && play_callback) {
        is_playing = true;
        play_callback();
    }
    if (is_playing) {
        ImGui::SameLine();
        ImGui::Text("Playing audio...");
    }
}

void GUIGeneralPanel::draw_song_info() {
    ImGui::Separator();
    ImGui::Text("Song Details");
    ImGui::InputText("Name", current_song.name, IM_ARRAYSIZE(current_song.name));
    draw_int_slider("BPM", current_song.bpm, 32, 1024);
    draw_float_slider("Normalizer", current_song.normalizer, 0.01f, 2.0f);
}

GUIGeneralPanel::GUIGeneralPanel()
    : is_playing(false), play_callback(nullptr) {
}

void GUIGeneralPanel::draw() {
    ImGui::Begin("General");
    from();
    draw_play_button();
    draw_song_info();
    to();
    ImGui::End();
}
