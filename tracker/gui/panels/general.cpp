#include "../../general.hpp"
#include "../../song/functions.hpp"
#include "../../structures/offsets.hpp"
#include "../../utils/string.hpp"
#include "../utils.hpp"
#include "general.hpp"

GUIGeneralPanel::GUIGeneralPanel(const bool visible, const bool windowed)
    : GUIPanel("General", visible, windowed) {
    initialize();
}

GUIElement GUIGeneralPanel::get_element() const {
    return GUIElement::General;
}

void GUIGeneralPanel::update() {
    current_song.edo = scale_composer.get_edo();
    current_song.a4_frequency = frequency_table.get_a4_frequency();
}

void GUIGeneralPanel::draw() {
    draw_play_button();
    draw_tabs();
}

void GUIGeneralPanel::from() {
    copy_string_to_buffer(song.get_title(), current_song.title, GUI_MAX_STRING_LENGTH);
    copy_string_to_buffer(song.get_author(), current_song.author, GUI_MAX_STRING_LENGTH);
    copy_string_to_buffer(song.get_message(), current_song.message, GUI_MAX_STRING_LENGTH);

    current_song.bpm = bpm;
    current_song.division = 240.0f / unit;
    current_song.sample_rate = sample_rate;
    current_song.normalizer = normalizer;
    current_song.output_channels = song.get_output_channels();
}

void GUIGeneralPanel::to() const {
    song.set_title(current_song.title);
    song.set_author(current_song.author);
    song.set_message(current_song.message);

    if (gui.is_playing()) {
        return;
    }

    bpm = current_song.bpm;
    unit = 240.0f / current_song.division;
    sample_rate = 4 * std::round(static_cast<float>(current_song.sample_rate) / 4);
    normalizer = current_song.normalizer;
    if (current_song.output_channels != song.get_output_channels()) {
        gui.stop();
        song.set_output_channels(current_song.output_channels);
    }

    calculate_ticks_per_beat();
}

void GUIGeneralPanel::validate_playback(const ValidationResult result, const int index) {
    if (result != ValidationResult::OK) {
        error = true;
        error_message = get_error_message(result, index);
    } else {
        error = false;
        error_message = "";
    }
}

void GUIGeneralPanel::play() {
    const auto [result, index] = gui.play();
    validate_playback(result, index);
}

void GUIGeneralPanel::play_from_current_page() {
    const auto [result, index] = gui.play_from(gui.get_current_page() * gui.get_page_size(), true);
    validate_playback(result, index);
}

void GUIGeneralPanel::play_from_current_position() {
    const int current_row = gui.get_current_row();
    if (current_row < 0) {
        return;
    }

    const auto [result, index] = gui.play_from(current_row, true);
    validate_playback(result, index);
}

void GUIGeneralPanel::draw_tabs() {
    if (gui.is_playing()) {
        if (gui.check_audio_error()) {
            error = true;
            error_message = "Audio error: segfault occurred during playback.";
            ImGui::OpenPopup("Error");
        }
    }

    if (ImGui::BeginTabBar("GeneralTabs")) {
        if (ImGui::BeginTabItem("Song")) {
            draw_song_info();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Tempo")) {
            draw_tempo();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Output")) {
            draw_output();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Tuning")) {
            draw_tuning_settings();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void GUIGeneralPanel::draw_play_button() {
    if (!gui.is_playing()) {
        draw_play_triangle();
    } else {
        draw_pause_rectangles();
    }
    ImGui::SameLine();
    draw_play_from_page_button();
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
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    const float sz = 20.0f;
    const ImVec2 center = ImVec2(p.x + sz * 0.5f, p.y + sz * 0.5f);

    const ImVec2 points[3] = {
        ImVec2(center.x - sz * 0.3f, center.y - sz * 0.5f),
        ImVec2(center.x + sz * 0.5f, center.y),
        ImVec2(center.x - sz * 0.3f, center.y + sz * 0.5f)
    };

    draw_list->AddTriangleFilled(points[0], points[1], points[2], GUI_BUTTON_COLOR_PLAY);

    ImGui::SetCursorScreenPos(p);
    ImGui::InvisibleButton("Play", ImVec2(sz, sz));
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Play (%s)", shortcut_manager.get_shortcut_display(ShortcutAction::PlayerPlayPause).c_str());
    }
    if (ImGui::IsItemClicked()) {
        play();
    }
}

void GUIGeneralPanel::draw_play_from_page_button() {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    const float sz = 20.0f;
    const ImVec2 center = ImVec2(p.x + sz * 0.5f, p.y + sz * 0.5f);

    const float line_x = center.x - sz * 0.3f;
    const float line_height = 0.8f * sz;
    const float bar_width = sz * 0.20f;
    const float padding = sz * 0.15f;
    const float height = sz * 0.05f;

    const ImVec2 left_min = ImVec2(p.x + padding - 3.0f, p.y + height);
    const ImVec2 left_max = ImVec2(p.x + padding - 3.0f + bar_width, p.y + sz - height);

    draw_list->AddRectFilled(left_min, left_max, GUI_BUTTON_COLOR_PLAY_FROM_CURRENT_POSITION);

    const ImVec2 points[3] = {
        ImVec2(center.x - sz * 0.3f + 3.0f, center.y - sz * 0.5f),
        ImVec2(center.x + sz * 0.5f + 3.0f, center.y),
        ImVec2(center.x - sz * 0.3f + 3.0f, center.y + sz * 0.5f)
    };

    draw_list->AddTriangleFilled(points[0], points[1], points[2], GUI_BUTTON_COLOR_PLAY_FROM_CURRENT_POSITION);

    ImGui::SetCursorScreenPos(p);
    ImGui::InvisibleButton("PlayFromPage", ImVec2(sz, sz));
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Play from current page (%s)", shortcut_manager.get_shortcut_display(ShortcutAction::PlayerPlayFromCurrentPage).c_str());
    }
    if (ImGui::IsItemClicked()) {
        play_from_current_page();
    }
}

void GUIGeneralPanel::draw_pause_rectangles() {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    const float sz = 20.0f;
    const float bar_width = sz * 0.20f;
    const float padding = sz * 0.15f;
    const float height = sz * 0.05f;

    const ImVec2 left_min = ImVec2(p.x + padding, p.y + height);
    const ImVec2 left_max = ImVec2(p.x + padding + bar_width, p.y + sz - height);
    const ImVec2 right_min = ImVec2(p.x + sz - padding - bar_width, p.y + height);
    const ImVec2 right_max = ImVec2(p.x + sz - padding, p.y + sz - height);

    draw_list->AddRectFilled(left_min, left_max, GUI_BUTTON_COLOR_PLAY);
    draw_list->AddRectFilled(right_min, right_max, GUI_BUTTON_COLOR_PLAY);

    ImGui::SetCursorScreenPos(p);
    ImGui::InvisibleButton("Pause", ImVec2(sz, sz));
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Pause (%s)", shortcut_manager.get_shortcut_display(ShortcutAction::PlayerPlayPause).c_str());
    }
    if (ImGui::IsItemClicked()) {
        play();
    }
}

void GUIGeneralPanel::draw_stop_square() const {
    ImVec2 p = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    const float sz = 20.0f;
    const float real_sz = sz * 0.85f;
    const ImVec2 p_min = ImVec2(p.x + sz * 0.5f - real_sz * 0.5f, p.y + sz * 0.5f - real_sz * 0.5f);
    const ImVec2 p_max = ImVec2(p.x + sz * 0.5f + real_sz * 0.5f, p.y + sz * 0.5f + real_sz * 0.5f);

    draw_list->AddRectFilled(p_min, p_max, GUI_BUTTON_COLOR_STOP);

    ImGui::SetCursorScreenPos(p);
    ImGui::InvisibleButton("Stop", ImVec2(sz, sz));
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
        ImGui::SetTooltip("Stop (%s)", shortcut_manager.get_shortcut_display(ShortcutAction::SongStop).c_str());
    }
    if (ImGui::IsItemClicked()) {
        gui.stop();
    }
}

void GUIGeneralPanel::draw_song_info() {
    ImGui::Text("Song details");
    draw_text(this, "Title", current_song.title, {Target::SPECIAL, 0, SPECIAL_TITLE});
    draw_text(this, "Author", current_song.author, {Target::SPECIAL, 0, SPECIAL_AUTHOR});
    draw_text(this, "Message", current_song.message, {Target::SPECIAL, 0, SPECIAL_MESSAGE});
}

void GUIGeneralPanel::draw_tempo() {
    ImGui::Text("Tempo");
    ImGui::BeginDisabled(gui.is_playing());
    draw_int_slider(this, "BPM", current_song.bpm, {Target::SPECIAL, 0, SPECIAL_BPM}, GUI_MIN_BPM, GUI_MAX_BPM);
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Real BPM: %s", std::to_string(song.calculate_real_bpm()).c_str());
    }
    draw_int_slider(this, "Division", current_song.division, {Target::SPECIAL, 0, SPECIAL_DIVISION}, GUI_MIN_DIVISION, GUI_MAX_DIVISION);
    ImGui::EndDisabled();
}

void GUIGeneralPanel::draw_output() {
    ImGui::Text("Output");
    ImGui::BeginDisabled(gui.is_playing());
    draw_float_slider(this, "Master gainer", current_song.normalizer, {Target::SPECIAL, 0, SPECIAL_MASTER_GAINER}, 0.01f, 2.0f);
    draw_float_slider(this, "Sample rate", current_song.sample_rate, {Target::SPECIAL, 0, SPECIAL_SAMPLE_RATE}, GUI_MIN_SAMPLE_RATE, GUI_MAX_SAMPLE_RATE, GUIScale::Linear, "%.0f");
    draw_int_slider(this, "Output channels", current_song.output_channels, {Target::SPECIAL, 0, SPECIAL_OUTPUT_CHANNELS}, 1, MAX_OUTPUT_CHANNELS);
    ImGui::EndDisabled();
}

void GUIGeneralPanel::draw_tuning_settings() {
    ImGui::Text("Tuning settings");
    draw_int_slider(this, "EDO", current_song.edo, {Target::SPECIAL, 0, SPECIAL_EDO}, MIN_EDO, MAX_EDO);
    draw_float_slider(this, "A4 frequency", current_song.a4_frequency, {Target::SPECIAL, 0, SPECIAL_A4_FREQUENCY}, MIN_A4_FREQUENCY, MAX_A4_FREQUENCY);

    if (draw_button("Apply tuning", 100.0f)) {
        gui.stop();
        song.change_tuning(current_song.edo, current_song.a4_frequency);
        gui.update();
    }
}

std::string GUIGeneralPanel::get_error_message(const ValidationResult result, const int index) const {
    std::ostringstream stream;
    switch (result) {
    case ValidationResult::OK:
        break;
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

    return stream.str();
}

void GUIGeneralPanel::draw_dialog_box() {
    if (error) {
        ImGui::OpenPopup("Error");
        error = false;
    }

    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup(error_message);
    }
}

void GUIGeneralPanel::register_shortcuts() {
    shortcut_manager.register_shortcut(
        ShortcutAction::PlayerPlayPause,
        [this]() { this->play(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PlayerPlayFromCurrentPosition,
        [this]() { this->play_from_current_position(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PlayerPlayFromCurrentPage,
        [this]() { this->play_from_current_page(); }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::SongStop,
        [this]() { gui.stop(); }
    );
}
