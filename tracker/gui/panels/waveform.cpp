#include <algorithm>
#include <cmath>

#include "../../general.hpp"
#include "../../song/core.hpp"
#include "../../song/song.hpp"
#include "../../tuning/frequencies.hpp"
#include "../../tuning/scale.hpp"
#include "../../utils/math.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "waveform.hpp"

GUIWaveformPanel::GUIWaveformPanel(const bool visible, const bool windowed)
    : GUIPanel("Waveform", visible, windowed) {
}

GUIElement GUIWaveformPanel::get_element() const {
    return GUIElement::Waveform;
}

void GUIWaveformPanel::update() {
    update_fft_sizes();
}

void GUIWaveformPanel::update_fft_sizes() {
    fft_sizes.clear();
    for (int size = GUI_MIN_FFT_SIZE; size <= GUI_MAX_FFT_SIZE; size *= 2) {
        fft_sizes.push_back(std::to_string(size));
    }

    fft_sizes_labels.clear();
    for (const auto &size : fft_sizes) {
        fft_sizes_labels.push_back(size.c_str());
    }
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
        history_size = static_cast<size_t>(clamp(hist_size, GUI_MIN_WAVEFORM_HISTORY_SIZE, GUI_MAX_WAVEFORM_HISTORY_SIZE));
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
            ("Output channel " + std::to_string(output_channel_index) + " (No data)").c_str()
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
    const size_t max_points = std::min(display_count, static_cast<size_t>(size.x * 2));
    const size_t step = display_count > max_points ? display_count / max_points : 1;
    const size_t actual_points = display_count / step;

    if (actual_points <= 1) {
        if (display_count > 0) {
            const float x = position.x + size.x / 2;
            const float y = zero_y - (history[start_index] * scale_y);
            draw_list->AddCircleFilled(ImVec2(x, y), 1.0f, waveform_color);
        }
    } else {
        const float scale_x = size.x / static_cast<float>(actual_points - 1);

        for (size_t i = 0; i < actual_points - 1; i++) {
            const size_t idx1 = start_index + i * step;
            const size_t idx2 = start_index + (i + 1) * step;

            if (idx2 >= history.size()) {
                break;
            }

            const float x1 = position.x + i * scale_x;
            const float y1 = zero_y - (history[idx1] * scale_y);
            const float x2 = position.x + (i + 1) * scale_x;
            const float y2 = zero_y - (history[idx2] * scale_y);

            draw_list->AddLine(
                ImVec2(x1, y1),
                ImVec2(x2, y2),
                waveform_color,
                1.5f
            );
        }
    }

    draw_list->AddText(
        ImVec2(position.x + 5, position.y + 5),
        IM_COL32(255, 255, 255, 255),
        ("Output channel " + std::to_string(output_channel_index)).c_str()
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
    if (ImGui::Combo("FFT size", &fft_size_index, fft_sizes_labels.data(), fft_sizes_labels.size())) {
        fft_parameters.fft_size = std::stoi(fft_sizes[fft_size_index]);
        fft_parameters.fft_size = std::max(GUI_MIN_FFT_SIZE, std::min(GUI_MAX_FFT_SIZE, fft_parameters.fft_size));
    }

    ImGui::SameLine();
    ImGui::SliderFloat("Min dB", &fft_parameters.min_db, GUI_MIN_FFT_DB, GUI_MAX_FFT_DB);

    ImGui::SameLine();
    ImGui::Checkbox("Wide bins", &fft_parameters.wide_bins);

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
            ("Spectrogram for Output channel " + std::to_string(output_channel_index) + " (No data)").c_str()
        );
        return;
    }

    gui.lock_audio_history();
    FFT fft(fft_parameters.fft_size);
    fft.compute(audio_history[output_channel_index]);
    const auto &magnitudes = fft.get_magnitudes();

    float max_magnitude = 0.001f;
    for (const auto &mag : magnitudes) {
        max_magnitude = std::max(max_magnitude, mag);
    }

    const float log_min = std::log10(GUI_MIN_SPECTROGRAM_FREQUENCY);
    const float log_max = std::log10(GUI_MAX_SPECTROGRAM_FREQUENCY);
    const float log_range = log_max - log_min;
    const int num_bins = magnitudes.size();

    const float fixed_bin_width = size.x / 200.0f;
    for (int i = 1; i < num_bins; i++) {
        const float frequency = i * sample_rate / (2.0f * num_bins);
        if (frequency < GUI_MIN_SPECTROGRAM_FREQUENCY || frequency > GUI_MAX_SPECTROGRAM_FREQUENCY) {
            continue;
        }

        const float log_frequency = std::log10(frequency);
        const float x_pos = position.x + ((log_frequency - log_min) / log_range) * size.x;

        const float next_frequency = (i + 1) * sample_rate / (2.0f * num_bins);
        const float next_log_frequency = std::log10(next_frequency);
        const float next_x_pos = position.x + ((next_log_frequency - log_min) / log_range) * size.x;

        float bin_width;
        float bin_x_pos;
        if (!fft_parameters.wide_bins) {
            bin_width = fixed_bin_width;
            bin_x_pos = x_pos - (bin_width / 2);
        } else {
            const float next_frequency = (i + 1) * sample_rate / (2.0f * num_bins);
            const float next_log_frequency = std::log10(next_frequency);
            const float next_x_pos = position.x + ((next_log_frequency - log_min) / log_range) * size.x;
            bin_width = std::max(1.0f, next_x_pos - x_pos);
            bin_x_pos = x_pos;
        }

        const float magnitude = magnitudes[i];
        float db = 20.0f * std::log10(magnitude / max_magnitude);
        db = clamp(db, fft_parameters.min_db, 0.0f);

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

        draw_list->AddRectFilled(
            ImVec2(x_pos, position.y + size.y),
            ImVec2(x_pos + bin_width, position.y + size.y - height),
            color
        );
    }

    for (const int frequency : frequencies) {
        const float log_frequency = std::log10(static_cast<float>(frequency));
        const float x_pos = position.x + (log_frequency - log_min) / log_range * size.x;

        draw_list->AddLine(
            ImVec2(x_pos, position.y),
            ImVec2(x_pos, position.y + 5),
            IM_COL32(180, 180, 180, 100)
        );

        const std::string frequency_name = get_frequency_name(frequency);
        draw_list->AddText(
            ImVec2(x_pos - 10, position.y + 7),
            IM_COL32(180, 180, 180, 150),
            frequency_name.c_str()
        );
    }

    draw_list->AddText(
        ImVec2(position.x + 5, position.y + 5),
        IM_COL32(255, 255, 255, 255),
        ("Output channel " + std::to_string(output_channel_index)).c_str()
    );

    gui.unlock_audio_history();

    if (ImGui::IsMouseHoveringRect(position, ImVec2(position.x + size.x, position.y + size.y))) {
        draw_frequency_tooltip(position, size, log_min, log_range);
    }
}

std::string GUIWaveformPanel::get_frequency_name(const int frequency) const {
    char label[16];
    if (frequency >= 1000) {
        snprintf(label, sizeof(label), "%dk", frequency / 1000);
    } else {
        snprintf(label, sizeof(label), "%d", frequency);
    }

    return std::string(label);
}

std::string GUIWaveformPanel::find_nearest_note(double frequency) {
    const double a4_frequency = frequency_table.get_a4_frequency();
    const int edo = scale_composer.get_edo();
    const int a4_index = frequency_table.get_a4_index();
    const uint8_t note = a4_index + std::round(std::log2(frequency / a4_frequency) * edo);
    return get_full_note_name(note);
}

void GUIWaveformPanel::draw_frequency_tooltip(const ImVec2 &position, const ImVec2 &size, const float log_min, const float log_range) {
    float mouse_x = ImGui::GetMousePos().x - position.x;

    float normalized_x = mouse_x / size.x;
    float log_freq = log_min + normalized_x * log_range;
    float freq = std::pow(10.0f, log_freq);

    const std::string note_name = find_nearest_note(freq);

    char tooltip[64];
    snprintf(tooltip, sizeof(tooltip), "%s\n%.2f Hz", note_name.c_str(), freq);

    ImGui::BeginTooltip();
    ImGui::Text("%s", tooltip);
    ImGui::EndTooltip();
}
