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
    const size_t output_channels_count = song.get_output_channels();
    if (output_channels_count == 0) {
        ImGui::Text("No output channels available.");
        return;
    }

    int hist_size = static_cast<int>(history_size);
    ImGui::PushItemWidth(200);
    if (ImGui::SliderInt("History size", &hist_size, GUI_MIN_WAVEFORM_HISTORY_SIZE, HISTORY_SIZE)) {
        history_size = static_cast<size_t>(std::clamp(hist_size, GUI_MIN_WAVEFORM_HISTORY_SIZE, HISTORY_SIZE));
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
}
