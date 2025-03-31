#include "wavetables.hpp"

GUIWavetablesPanel::GUIWavetablesPanel() {
    from_wavetable();
    update();
}

void GUIWavetablesPanel::draw() {
    ImGui::Begin("Wavetable Editor");

    if (wavetables.empty()) {
        ImGui::Text("No wavetables available.");
        ImGui::End();
        return;
    }

    prepare_combo(wavetable_names, "##WavetableCombo", wavetable_index);
    from_wavetable();
    draw_waveform();
    to_wavetable();

    ImGui::End();
}

bool GUIWavetablesPanel::is_index_valid() const {
    return wavetable_index >= 0 && wavetable_index < wavetables.size();
}

void GUIWavetablesPanel::from_wavetable() {
    if (!is_index_valid()) {
        return;
    }

    const Wavetable *wavetable = wavetables[wavetable_index];
    current_wavetable.size = wavetable->wavetable_size;
    current_wavetable.interpolate = false;
    if (current_wavetable.wave.size() != current_wavetable.size) {
        current_wavetable.wave.resize(std::min(current_wavetable.size, max_points));
    }

    for (size_t i = 0; i < current_wavetable.wave.size(); ++i) {
        current_wavetable.wave[i] = (2.0f * wavetable->wavetable[i]) / UINT8_MAX - 1.0f;
    }
}

void GUIWavetablesPanel::to_wavetable() {
    if (!is_index_valid()) {
        return;
    }

    Wavetable *wavetable = wavetables[wavetable_index];
    const size_t size = current_wavetable.size;
    const size_t structure_size = size + 1;

    Wavetable *new_wavetable = static_cast<Wavetable *>(operator new(structure_size));
    new_wavetable->wavetable_size = size;
    for (size_t i = 0; i < size; ++i) {
        const uint8_t value = std::round((current_wavetable.wave[i] + 1.0f) * UINT8_MAX / 2.0f);
        new_wavetable->wavetable[i] = value;
    }

    if (new_wavetable->wavetable_size > wavetable->wavetable_size) {
        for (size_t i = wavetable->wavetable_size; i < new_wavetable->wavetable_size; ++i) {
            new_wavetable->wavetable[i] = 0x80;
        }
    }

    wavetables[wavetable_index] = new_wavetable;
    delete wavetable;
}

void GUIWavetablesPanel::draw_wavetable_length() {
    int old_size = current_wavetable.size;
    draw_number_of_items("Points", "##WavetableLength", current_wavetable.size, 1, max_points);

    if (old_size != current_wavetable.size) {
        current_wavetable.wave.resize(current_wavetable.size);
        if (current_wavetable.size > old_size) {
            for (int i = old_size; i < current_wavetable.size; i++) {
                current_wavetable.wave[i] = 0.0f;
            }
        }
    }
}

void GUIWavetablesPanel::draw_waveform() {
    ImGui::Separator();
    ImGui::Checkbox("Interpolate", &current_wavetable.interpolate);

    if (wavetables.empty()) {
        ImGui::Text("No wavetables available.");
        return;
    }

    ImGui::Text("Waveform:");
    if (current_wavetable.wave.empty()) {
        ImGui::Text("No data to display.");
        return;
    }

    draw_wavetable_length();

    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const float width = std::max(45.0f, ImGui::GetContentRegionAvail().x - 35.0f);
    const float height = std::max(20.0f, ImGui::GetContentRegionAvail().y - 10.0f);
    const ImVec2 size = ImVec2(width, height);
    ImGui::InvisibleButton("wave_canvas", size);
    const ImVec2 canvas_p0 = p;
    const ImVec2 canvas_p1 = ImVec2(p.x + size.x, p.y + size.y);

    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(200, 200, 200, 255));

    const size_t data_size = current_wavetable.wave.size();
    const float x_step = size.x / data_size;
    const float y_center = canvas_p0.y + size.y / 2.0f;
    const int grid_lines = 4;

    ImGuiIO &io = ImGui::GetIO();
    ImVec2 mouse_pos = io.MousePos;
    bool is_hovered = ImGui::IsItemHovered();
    bool is_active = ImGui::IsItemActive();

    if (is_hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        const float relative_x = mouse_pos.x - canvas_p0.x;
        int index = static_cast<int>(std::round(relative_x / x_step));
        if (index >= 0 && index < static_cast<int>(data_size)) {
            const int current_point = index;
            const float relative_y = mouse_pos.y - canvas_p0.y;
            float normalized = 1.0f - (relative_y / size.y);
            normalized = std::clamp(normalized, 0.0f, 1.0f);
            const float value = (normalized * 2.0f) - 1.0f;
            current_wavetable.wave[current_point] = value;
        }
    }

    for (int i = 0; i <= grid_lines; ++i) {
        const float y = canvas_p0.y + size.y * (1.0f - i / static_cast<float>(grid_lines));
        const float label_value = -1.0f + 2.0f * i / grid_lines;
        char label[5];
        snprintf(label, sizeof(label), "%.1f", label_value);

        const ImVec2 text_size = ImGui::CalcTextSize(label);
        draw_list->AddLine(ImVec2(canvas_p0.x, y), ImVec2(canvas_p1.x, y), IM_COL32(100, 100, 100, 255), 1.0f);
        draw_list->AddText(ImVec2(40.0f + canvas_p1.x - text_size.x - 5, y - text_size.y / 2), IM_COL32(255, 255, 255, 255), label);
    }

    if (data_size < 2) {
        return;
    }

    for (size_t i = 0; i < data_size; ++i) {
        const float x1 = canvas_p0.x + i * x_step;
        const float y1 = y_center - current_wavetable.wave[i] * (size.y / 2.0f);
        const float x2 = canvas_p0.x + (i + 1) * x_step;
        const float y2 = y_center - current_wavetable.wave[i + 1 == data_size ? 0 : i + 1] * (size.y / 2.0f);

        draw_list->AddCircleFilled(ImVec2(x1, y1), 3.0f, IM_COL32(0, 255, 0, 255));
        if (current_wavetable.interpolate) {
            draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(0, 255, 0, 255), 1.0f);
        } else {
            draw_list->AddCircleFilled(ImVec2(x2, y1), 3.0f, IM_COL32(0, 255, 0, 255));
            draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y1), IM_COL32(0, 255, 0, 255), 1.0f);
            draw_list->AddLine(ImVec2(x2, y1), ImVec2(x2, y2), IM_COL32(0, 255, 0, 255), 1.0f);
        }
    }
}

void GUIWavetablesPanel::update() {
    update_items(wavetable_names, wavetables.size(), "Wavetable ", wavetable_index);
}
