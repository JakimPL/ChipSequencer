#include <algorithm>
#include <cmath>

#include "../../general.hpp"
#include "../../song/core.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "waveform.hpp"

GUIWaveformPanel::GUIWaveformPanel(const bool visible, const bool windowed)
    : GUIPanel("Waveform", visible, windowed) {
    initialize();
}

GUIElement GUIWaveformPanel::get_element() const {
    return GUIElement::Waveform;
}

void GUIWaveformPanel::draw() {
    if (ImGui::BeginTabBar("WaveformPanelTabBar")) {
        if (ImGui::BeginTabItem("Waveform")) {
            draw_waveform();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Spectrogram")) {
            draw_spectrogram();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void GUIWaveformPanel::draw_waveform() {
    if (ImGui::IsWindowCollapsed()) {
        return;
    }

    const size_t output_channels_count = song.get_output_channels();
    if (output_channels_count == 0) {
        ImGui::Text("No output channels available.");
        return;
    }

    int hist_size = static_cast<int>(history_size);
    ImGui::PushItemWidth(200);
    if (ImGui::SliderInt("History size", &hist_size, GUI_MIN_WAVEFORM_HISTORY_SIZE, GUI_MAX_WAVEFORM_HISTORY_SIZE)) {
        history_size = static_cast<size_t>(std::clamp(hist_size, GUI_MIN_WAVEFORM_HISTORY_SIZE, GUI_MAX_WAVEFORM_HISTORY_SIZE));
    }
    ImGui::PopItemWidth();

    const ImVec2 available_size = ImGui::GetContentRegionAvail();
    const float channel_height = available_size.y / output_channels_count;

    for (size_t i = 0; i < output_channels_count; i++) {
        ImGui::PushID(static_cast<int>(i));

        const ImVec2 channel_size(available_size.x, channel_height - 4);
        const ImVec2 channel_pos = ImGui::GetCursorScreenPos();

        draw_channel_waveform(i, channel_size, channel_pos);

        ImGui::Dummy(channel_size);
        ImGui::PopID();
    }
}

void GUIWaveformPanel::draw_channel_waveform(const int output_channel_index, const ImVec2 &size, const ImVec2 &position) {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(
        position,
        ImVec2(position.x + size.x, position.y + size.y),
        IM_COL32(20, 20, 20, 255)
    );

    draw_list->AddRect(
        position,
        ImVec2(position.x + size.x, position.y + size.y),
        IM_COL32(60, 60, 60, 255)
    );

    ImU32 waveform_color;
    switch (output_channel_index) {
    case 0:
        waveform_color = GUI_WAVEFORM_COLOR_1;
        break;
    case 1:
        waveform_color = GUI_WAVEFORM_COLOR_2;
        break;
    case 2:
        waveform_color = GUI_WAVEFORM_COLOR_3;
        break;
    case 3:
        waveform_color = GUI_WAVEFORM_COLOR_4;
        break;
    }

    const auto &audio_history = gui.get_audio_history();
    if (output_channel_index >= audio_history.size() || audio_history[output_channel_index].empty()) {
        draw_list->AddText(
            ImVec2(position.x + 5, position.y + 5),
            IM_COL32(255, 255, 255, 255),
            ("Channel " + std::to_string(output_channel_index) + " (No data)").c_str()
        );
        return;
    }

    gui.lock_audio_history();
    const auto &history = audio_history[output_channel_index];
    size_t display_count = std::min(history_size, history.size());
    size_t start_index = history.size() - display_count;

    const float zero_y = position.y + size.y / 2;
    draw_list->AddLine(
        ImVec2(position.x, zero_y),
        ImVec2(position.x + size.x, zero_y),
        IM_COL32(100, 100, 100, 255)
    );

    draw_list->AddLine(
        ImVec2(position.x, position.y + 2),
        ImVec2(position.x + size.x, position.y + 2),
        IM_COL32(70, 70, 70, 255)
    );
    draw_list->AddLine(
        ImVec2(position.x, position.y + size.y - 2),
        ImVec2(position.x + size.x, position.y + size.y - 2),
        IM_COL32(70, 70, 70, 255)
    );

    const float scale_y = (size.y - 4) / 2.0f;
    const float scale_x = size.x / static_cast<float>(display_count);
    for (size_t i = 0; i < display_count - 1; i++) {
        const float x1 = position.x + i * scale_x;
        const float y1 = zero_y - (history[start_index + i] * scale_y);
        const float x2 = position.x + (i + 1) * scale_x;
        const float y2 = zero_y - (history[start_index + i + 1] * scale_y);

        draw_list->AddLine(
            ImVec2(x1, y1),
            ImVec2(x2, y2),
            waveform_color,
            1.5f
        );
    }

    draw_list->AddText(
        ImVec2(position.x + 5, position.y + 5),
        IM_COL32(255, 255, 255, 255),
        ("Channel " + std::to_string(output_channel_index)).c_str()
    );

    if (!history.empty()) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Value: %.2f", history.back());
        draw_list->AddText(
            ImVec2(position.x + size.x - 80, position.y + 5),
            IM_COL32(255, 255, 255, 255),
            buffer
        );
    }

    gui.unlock_audio_history();
}

void GUIWaveformPanel::draw_spectrogram() {
    if (ImGui::IsWindowCollapsed()) {
        return;
    }

    const size_t output_channels_count = song.get_output_channels();
    if (output_channels_count == 0) {
        ImGui::Text("No output channels available.");
        return;
    }

    ImGui::PushItemWidth(150);
    if (ImGui::SliderInt("FFT size", &fft_parameters.fft_size, GUI_MIN_FFT_SIZE, GUI_MAX_FFT_SIZE)) {
        fft_parameters.fft_size = 1 << (int) std::log2(fft_parameters.fft_size);
    }
    ImGui::SameLine();
    ImGui::SliderFloat("Min dB", &fft_parameters.min_db, GUI_MIN_FFT_DB, GUI_MAX_FFT_DB);
    ImGui::PopItemWidth();

    const ImVec2 available_size = ImGui::GetContentRegionAvail();
    const float channel_height = available_size.y / output_channels_count;

    for (size_t i = 0; i < output_channels_count; i++) {
        ImGui::PushID(static_cast<int>(i));
        const ImVec2 channel_size(available_size.x, channel_height - 4);
        const ImVec2 channel_pos = ImGui::GetCursorScreenPos();
        draw_channel_spectrogram(i, channel_size, channel_pos);
        ImGui::Dummy(channel_size);
        ImGui::PopID();
    }
}

void GUIWaveformPanel::draw_channel_spectrogram(const int output_channel_index, const ImVec2 &size, const ImVec2 &position) {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(
        position,
        ImVec2(position.x + size.x, position.y + size.y),
        IM_COL32(20, 20, 20, 255)
    );

    draw_list->AddRect(
        position,
        ImVec2(position.x + size.x, position.y + size.y),
        IM_COL32(60, 60, 60, 255)
    );

    const auto &audio_history = gui.get_audio_history();
    if (output_channel_index >= audio_history.size() || audio_history[output_channel_index].empty()) {
        draw_list->AddText(
            ImVec2(position.x + 5, position.y + 5),
            IM_COL32(255, 255, 255, 255),
            ("Spectrogram for Channel " + std::to_string(output_channel_index) + " (No data)").c_str()
        );
        return;
    }

    gui.lock_audio_history();
    if (fft.get_size() != fft_parameters.fft_size) {
        // TODO: we need to reinitialize the FFT with the new size
    }

    fft.compute(audio_history[output_channel_index]);
    const auto &magnitudes = fft.get_magnitudes();

    float max_magnitude = 0.001f;
    for (const auto &mag : magnitudes) {
        max_magnitude = std::max(max_magnitude, mag);
    }

    const float log_min = std::log10(20.0f);
    const float log_max = std::log10(20000.0f);
    const float log_range = log_max - log_min;
    const int num_bins = magnitudes.size();
    const float sample_rate = 44100.0f;

    for (int i = 1; i < num_bins; i++) {
        const float freq = i * sample_rate / (2.0f * num_bins);
        if (freq < 20.0f || freq > 20000.0f) {
            continue;
        }

        const float log_freq = std::log10(freq);
        const float x_pos = position.x + ((log_freq - log_min) / log_range) * size.x;

        const float magnitude = magnitudes[i];
        float db = 20.0f * std::log10(magnitude / max_magnitude);
        db = std::clamp(db, fft_parameters.min_db, 0.0f);

        const float normalized_db = (db - fft_parameters.min_db) / -fft_parameters.min_db;
        const float height = normalized_db * size.y;

        ImU32 color;
        if (normalized_db < 0.2f) {
            const float t = normalized_db / 0.2f;
            color = IM_COL32(0, (int) (255 * t), 255, 255);
        } else if (normalized_db < 0.4f) {
            const float t = (normalized_db - 0.2f) / 0.2f;
            color = IM_COL32(0, 255, (int) (255 * (1 - t)), 255);
        } else if (normalized_db < 0.6f) {
            const float t = (normalized_db - 0.4f) / 0.2f;
            color = IM_COL32((int) (255 * t), 255, 0, 255);
        } else if (normalized_db < 0.8f) {
            const float t = (normalized_db - 0.6f) / 0.2f;
            color = IM_COL32(255, (int) (255 * (1 - t)), 0, 255);
        } else {
            const float t = (normalized_db - 0.8f) / 0.2f;
            color = IM_COL32(255, (int) (255 * t), (int) (255 * t), 255);
        }

        const float bin_width = std::max(1.0f, (size.x / log_range) / 100.0f);
        draw_list->AddRectFilled(
            ImVec2(x_pos, position.y + size.y),
            ImVec2(x_pos + bin_width, position.y + size.y - height),
            color
        );
    }

    const int freq_labels[] = {50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000};
    for (int freq : freq_labels) {
        const float log_freq = std::log10(static_cast<float>(freq));
        const float x_pos = position.x + (log_freq - log_min) / log_range * size.x;

        draw_list->AddLine(
            ImVec2(x_pos, position.y + size.y),
            ImVec2(x_pos, position.y + size.y - 5),
            IM_COL32(180, 180, 180, 100)
        );

        char label[16];
        if (freq >= 1000) {
            snprintf(label, sizeof(label), "%dk", freq / 1000);
        } else {
            snprintf(label, sizeof(label), "%d", freq);
        }

        draw_list->AddText(
            ImVec2(x_pos - 10, position.y + size.y - 15),
            IM_COL32(180, 180, 180, 150),
            label
        );
    }

    draw_list->AddText(
        ImVec2(position.x + 5, position.y + 5),
        IM_COL32(255, 255, 255, 255),
        ("Channel " + std::to_string(output_channel_index)).c_str()
    );

    gui.unlock_audio_history();
}
