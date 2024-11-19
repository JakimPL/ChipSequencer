#ifndef GUI_PANELS_WAVETABLES_HPP
#define GUI_PANELS_WAVETABLES_HPP

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include "../init.hpp"
#include "utils.hpp"

class GUIWavetablesPanel {
  private:
    struct CurrentWavetable {
        int size;
        std::vector<float> wave;
    } current_wavetable;

    int wavetable_index = 0;
    const int max_points = 128;
    std::vector<std::string> wavetable_names;

    void from_wavetable() {
        const Wavetable *wavetable = wavetables[wavetable_index];
        current_wavetable.size = wavetable->wavetable_size;
        current_wavetable.wave.resize(current_wavetable.size);
        for (size_t i = 0; i < current_wavetable.wave.size(); ++i) {
            current_wavetable.wave[i] = (2.0f * wavetable->wavetable[i]) / UINT8_MAX - 1.0f;
        }
    }

    void to_wavetable() {
        Wavetable *wavetable = wavetables[wavetable_index];
        const size_t size = current_wavetable.size;
        const size_t structure_size = size + 1;

        Wavetable *new_wavetable = static_cast<Wavetable *>(operator new(structure_size));
        new_wavetable->wavetable_size = size;
        for (size_t i = 0; i < size; ++i) {
            const uint8_t value = std::round((current_wavetable.wave[i] + 1.0f) * UINT8_MAX / 2.0f);
            new_wavetable->wavetable[i] = value;
        }

        wavetables[wavetable_index] = new_wavetable;
    }

    void draw_wavetable_length() {
        draw_number_of_items("Points", "##WavetableLength", current_wavetable.size, 1, max_points);
    }

    void draw_waveform() {
        ImGui::Separator();
        ImGui::Text("Waveform:");

        if (current_wavetable.wave.empty()) {
            ImGui::Text("No data to display.");
            return;
        }

        ImGui::BeginChild("WaveformScroll", ImVec2(0, 200), true);
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 p = ImGui::GetCursorScreenPos();
        const float width = std::max(30.0f, ImGui::GetContentRegionAvail().x - 25.0f);
        const float height = ImGui::GetContentRegionAvail().y;
        ImVec2 size = ImVec2(width, height);
        ImGui::InvisibleButton("wave_canvas", size);
        ImVec2 canvas_p0 = p;
        ImVec2 canvas_p1 = ImVec2(p.x + size.x, p.y + size.y);

        draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(200, 200, 200, 255));

        const size_t data_size = current_wavetable.wave.size();
        const float x_step = size.x / (data_size - 1);
        const float y_center = canvas_p0.y + size.y / 2.0f;
        const int grid_lines = 4;

        for (int i = 0; i <= grid_lines; ++i) {
            const float y = canvas_p0.y + size.y * (1.0f - i / static_cast<float>(grid_lines));
            float label_value = -1.0f + 2.0f * i / grid_lines;
            // Format label with one decimal place
            char label[5];
            snprintf(label, sizeof(label), "%.1f", label_value);

            ImVec2 text_size = ImGui::CalcTextSize(label);
            draw_list->AddLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(100, 100, 100, 255), 1.0f);
            draw_list->AddText(ImVec2(30.0f + canvas_p1.x - text_size.x - 5, y - text_size.y / 2), IM_COL32(255, 255, 255, 255), label);
        }

        if (data_size < 2) {
            ImGui::EndChild();
            return;
        }

        for (size_t i = 0; i < data_size - 1; ++i) {
            float x1 = canvas_p0.x + i * x_step;
            float y1 = y_center - current_wavetable.wave[i] * (size.y / 2.0f);
            float x2 = canvas_p0.x + (i + 1) * x_step;
            float y2 = y_center - current_wavetable.wave[i + 1] * (size.y / 2.0f);
            draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(0, 255, 0, 255), 1.0f);
        }

        for (size_t i = 0; i < data_size; ++i) {
            float x = canvas_p0.x + i * x_step;
            float y = y_center - current_wavetable.wave[i] * (size.y / 2.0f);
            draw_list->AddCircleFilled(ImVec2(x, y), 3.0f, IM_COL32(0, 255, 0, 255));
        }

        ImGui::EndChild();
    }

    void update_wavetables() {
        update_items(wavetable_names, wavetables.size(), "Wavetable ", wavetable_index);
    }

  public:
    GUIWavetablesPanel() {
        update_wavetables();
    }

    void draw() {
        ImGui::Begin("Wavetable Editor");

        if (wavetables.empty()) {
            ImGui::Text("No wavetables available.");
            ImGui::End();
            return;
        }

        prepare_combo(wavetable_names, "##WavetableCombo", wavetable_index);
        from_wavetable();

        draw_wavetable_length();
        draw_waveform();

        to_wavetable();

        ImGui::End();
    }
};

#endif // GUI_PANELS_WAVETABLES_HPP
