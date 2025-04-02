#include "../../general.hpp"
#include "general.hpp"

GUIGeneralPanel::GUIGeneralPanel() {
}

void GUIGeneralPanel::draw() {
    ImGui::Begin("General");
    from();
    draw_play_button();
    draw_song_info();
    check_keyboard_input();
    to();
    ImGui::End();
}

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
    if (!gui.is_playing()) {
        draw_play_triangle();
    } else {
        draw_pause_rectangles();
    }

    if (gui.is_playing()) {
        ImGui::SameLine();
        ImGui::Text("Playing audio...");
    } else if (gui.is_paused()) {
        ImGui::SameLine();
        ImGui::Text("Paused");
    } else {
        ImGui::SameLine();
        ImGui::Text("Stopped");
    }
}

void GUIGeneralPanel::draw_play_triangle() const {
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
    ImGui::InvisibleButton("Play", ImVec2(sz, sz));
    if (ImGui::IsItemClicked()) {
        gui.play();
    }
}

void GUIGeneralPanel::draw_pause_rectangles() const {
    ImVec2 p = ImGui::GetCursorScreenPos();
    float sz = 20.0f;
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    float bar_width = sz * 0.20f;
    float padding = sz * 0.15f;
    float height = sz * 0.05f;

    ImVec2 left_min = ImVec2(p.x + padding, p.y + height);
    ImVec2 left_max = ImVec2(p.x + padding + bar_width, p.y + sz - height);
    ImVec2 right_min = ImVec2(p.x + sz - padding - bar_width, p.y + height);
    ImVec2 right_max = ImVec2(p.x + sz - padding, p.y + sz - height);

    draw_list->AddRectFilled(left_min, left_max, IM_COL32(0, 255, 0, 255));
    draw_list->AddRectFilled(right_min, right_max, IM_COL32(0, 255, 0, 255));

    ImGui::SetCursorScreenPos(p);
    ImGui::InvisibleButton("Pause", ImVec2(sz, sz));
    if (ImGui::IsItemClicked()) {
        gui.play();
    }
}

void GUIGeneralPanel::draw_song_info() {
    ImGui::Separator();
    ImGui::Text("Song Details");
    ImGui::InputText("Name", current_song.name, IM_ARRAYSIZE(current_song.name));
    draw_int_slider("BPM", current_song.bpm, GUI_MIN_BPM, GUI_MAX_BPM);
    draw_float_slider("Normalizer", current_song.normalizer, 0.01f, 2.0f);
}
