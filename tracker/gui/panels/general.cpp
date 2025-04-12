#include "../../general.hpp"
#include "../../song/functions.hpp"
#include "../../utils/string.hpp"
#include "../utils.hpp"
#include "general.hpp"

GUIGeneralPanel::GUIGeneralPanel() {
}

void GUIGeneralPanel::update() {
    current_song.edo = scale_composer.get_edo();
    current_song.a4_frequency = frequency_table.get_a4_frequency();
}

void GUIGeneralPanel::draw() {
    ImGui::Begin("General");
    from();
    draw_play_button();
    draw_song_info();
    draw_tuning_settings();
    check_keyboard_input();
    to();

    if (error) {
        ImGui::OpenPopup("Error");
        error = false;
    }

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup(error_message);
    }

    ImGui::End();
}

void GUIGeneralPanel::from() {
    string_copy_to_buffer(song.get_title(), current_song.title, GUI_MAX_STRING_LENGTH);
    string_copy_to_buffer(song.get_author(), current_song.author, GUI_MAX_STRING_LENGTH);

    current_song.bpm = bpm;
    current_song.division = 240.0f / unit;
    current_song.sample_rate = sample_rate;
    current_song.normalizer = normalizer;
    current_song.output_channels = song.get_output_channels();
}

void GUIGeneralPanel::to() const {
    song.set_title(current_song.title);
    song.set_author(current_song.author);

    sample_rate = 4 * std::round(static_cast<float>(current_song.sample_rate) / 4);
    unit = 240.0f / current_song.division;
    normalizer = current_song.normalizer;
    if (current_song.output_channels != song.get_output_channels()) {
        song.set_output_channels(current_song.output_channels);
    }

    if (current_song.bpm != bpm) {
        bpm = current_song.bpm;
        calculate_ticks_per_beat();
    }
}

void GUIGeneralPanel::play() {
    const auto [result, index] = song.validate();
    if (result != ValidationResult::OK) {
        error = true;
        std::ostringstream stream;
        switch (result) {
        case ValidationResult::InvalidSongLength:
            stream << "Song is empty!";
            break;
        case ValidationResult::OscillatorMissingWavetable:
            stream << "Oscillator " << index << " is missing a wavetable.";
            break;
        case ValidationResult::OrderMissingSequence:
            stream << "Order " << index << " is missing a sequence.";
            break;
        case ValidationResult::ChannelMissingOscillator:
            stream << "Channel " << index << " is missing an oscillator.";
            break;
        case ValidationResult::ChannelMissingEnvelope:
            stream << "Channel " << index << " is missing an envelope.";
            break;
        case ValidationResult::ChannelMissingOrder:
            stream << "Channel " << index << " is missing order.";
            break;
        }

        error_message = stream.str();
    } else {
        error = false;
        error_message = "";
        gui.play();
    }
}

void GUIGeneralPanel::draw_play_button() {
    if (!gui.is_playing()) {
        draw_play_triangle();
    } else {
        draw_pause_rectangles();
    }
    ImGui::SameLine();
    draw_stop_square();

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

void GUIGeneralPanel::draw_play_triangle() {
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
        play();
    }
}

void GUIGeneralPanel::draw_pause_rectangles() {
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
        play();
    }
}

void GUIGeneralPanel::draw_stop_square() const {
    ImVec2 p = ImGui::GetCursorScreenPos();
    float sz = 20.0f;
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    ImVec2 p_min = p;
    ImVec2 p_max = ImVec2(p.x + sz, p.y + sz);
    draw_list->AddRectFilled(p_min, p_max, IM_COL32(255, 0, 0, 255));

    ImGui::SetCursorScreenPos(p);
    ImGui::InvisibleButton("Stop", ImVec2(sz, sz));
    if (ImGui::IsItemClicked()) {
        gui.stop();
    }
}

void GUIGeneralPanel::draw_song_info() {
    ImGui::Separator();
    ImGui::Text("Song Details");
    ImGui::InputText("Title", current_song.title, IM_ARRAYSIZE(current_song.title));
    ImGui::InputText("Author", current_song.author, IM_ARRAYSIZE(current_song.author));

    ImGui::Separator();
    ImGui::Text("Tempo");
    draw_int_slider("BPM", current_song.bpm, {}, GUI_MIN_BPM, GUI_MAX_BPM);
    draw_int_slider("Division", current_song.division, {}, GUI_MIN_UNIT, GUI_MAX_UNIT);

    ImGui::Separator();
    ImGui::Text("Output");
    draw_float_slider("Sample rate", current_song.sample_rate, {}, GUI_MIN_SAMPLE_RATE, GUI_MAX_SAMPLE_RATE, GUIScale::Linear, "%.0f");
    draw_float_slider("Normalizer", current_song.normalizer, {}, 0.01f, 2.0f);
    draw_int_slider("Output channels", current_song.output_channels, {}, 1, MAX_OUTPUT_CHANNELS);
}

void GUIGeneralPanel::draw_tuning_settings() {
    ImGui::Separator();
    ImGui::Text("Tuning Settings");
    draw_int_slider("EDO", current_song.edo, {}, MIN_EDO, MAX_EDO);
    draw_float_slider("A4 Frequency", current_song.a4_frequency, {}, MIN_A4_FREQUENCY, MAX_A4_FREQUENCY);

    if (draw_button("Apply Tuning", 100.0f)) {
        gui.stop();
        song.change_tuning(current_song.edo, current_song.a4_frequency);
        gui.update();
    }
}
