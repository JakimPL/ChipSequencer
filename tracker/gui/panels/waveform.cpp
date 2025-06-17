#include "waveform.hpp"
#include "../../general.hpp"
#include "../../song/core.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include <algorithm>
#include <cmath>

GUIWaveformPanel::GUIWaveformPanel(const bool visible, const bool windowed)
    : GUIPanel("Waveform", visible, windowed) {
    initialize();
}

GUIElement GUIWaveformPanel::get_element() const {
    return GUIElement::Waveform;
}

void GUIWaveformPanel::from() {
    const size_t output_channels_count = song.get_output_channels();
    channel_histories.resize(output_channels_count);
}

void GUIWaveformPanel::update() {
    update_histories();
}

void GUIWaveformPanel::update_histories() {
    if (!gui.is_playing()) {
        return;
    }

    const size_t output_channels_count = song.get_output_channels();
    for (size_t i = 0; i < output_channels_count; i++) {
        float sample = std::clamp(output[i], -1.0f, 1.0f);

        auto &history = channel_histories[i];
        history.push_back(sample);

        if (history.size() > history_size) {
            history.pop_front();
        }
    }
}

void GUIWaveformPanel::draw() {
    update_histories();

    const size_t output_channels_count = song.get_output_channels();
    if (output_channels_count == 0) {
        ImGui::Text("No output channels available.");
        return;
    }

    int hist_size = static_cast<int>(history_size);
    ImGui::PushItemWidth(200);
    if (ImGui::SliderInt("History Size", &hist_size, 128, 8192 * 16)) {
        history_size = static_cast<size_t>(std::clamp(hist_size, 128, 8192));
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

void GUIWaveformPanel::draw_channel_waveform(int channel_index, const ImVec2 &size, const ImVec2 &pos) {
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        IM_COL32(20, 20, 20, 255)
    );

    draw_list->AddRect(
        pos,
        ImVec2(pos.x + size.x, pos.y + size.y),
        IM_COL32(60, 60, 60, 255)
    );

    ImU32 waveform_color;
    switch (channel_index % 4) {
    case 0:
        waveform_color = IM_COL32(0, 200, 0, 255);
        break;
    case 1:
        waveform_color = IM_COL32(200, 0, 0, 255);
        break;
    case 2:
        waveform_color = IM_COL32(0, 0, 200, 255);
        break;
    case 3:
        waveform_color = IM_COL32(200, 200, 0, 255);
        break;
    default:
        waveform_color = IM_COL32(200, 200, 200, 255);
        break;
    }

    const auto &history = channel_histories[channel_index];
    if (history.empty()) {
        draw_list->AddText(
            ImVec2(pos.x + 5, pos.y + 5),
            IM_COL32(255, 255, 255, 255),
            ("Channel " + std::to_string(channel_index) + " (No data)").c_str()
        );
        return;
    }

    const float zero_y = pos.y + size.y / 2;
    draw_list->AddLine(
        ImVec2(pos.x, zero_y),
        ImVec2(pos.x + size.x, zero_y),
        IM_COL32(100, 100, 100, 255)
    );

    draw_list->AddLine(
        ImVec2(pos.x, pos.y + 2),
        ImVec2(pos.x + size.x, pos.y + 2),
        IM_COL32(70, 70, 70, 255)
    );
    draw_list->AddLine(
        ImVec2(pos.x, pos.y + size.y - 2),
        ImVec2(pos.x + size.x, pos.y + size.y - 2),
        IM_COL32(70, 70, 70, 255)
    );

    const float scale_y = (size.y - 4) / 2.0f; // Leave small margins
    const float scale_x = size.x / static_cast<float>(history.size());

    std::vector<ImVec2> points;
    points.reserve(history.size() * 2 + 2);

    points.push_back(ImVec2(pos.x, zero_y));

    for (size_t i = 0; i < history.size(); i++) {
        const float x = pos.x + i * scale_x;
        const float y = zero_y - (history[i] * scale_y);
        points.push_back(ImVec2(x, y));
    }

    points.push_back(ImVec2(pos.x + size.x, zero_y));

    draw_list->AddConvexPolyFilled(
        points.data(),
        static_cast<int>(points.size()),
        IM_COL32((waveform_color >> 16) & 0xFF, (waveform_color >> 8) & 0xFF, (waveform_color) & 0xFF, 50)
    );

    for (size_t i = 0; i < history.size() - 1; i++) {
        const float x1 = pos.x + i * scale_x;
        const float y1 = zero_y - (history[i] * scale_y);
        const float x2 = pos.x + (i + 1) * scale_x;
        const float y2 = zero_y - (history[i + 1] * scale_y);

        draw_list->AddLine(
            ImVec2(x1, y1),
            ImVec2(x2, y2),
            waveform_color,
            1.5f
        );
    }

    draw_list->AddText(
        ImVec2(pos.x + 5, pos.y + 5),
        IM_COL32(255, 255, 255, 255),
        ("Channel " + std::to_string(channel_index)).c_str()
    );

    if (!history.empty()) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "Value: %.2f", history.back());
        draw_list->AddText(
            ImVec2(pos.x + size.x - 80, pos.y + 5),
            IM_COL32(255, 255, 255, 255),
            buffer
        );
    }
}
