#include "../../general.hpp"
#include "../enums.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "wavetables.hpp"

GUIWavetablesPanel::GUIWavetablesPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUIWavetablesPanel::draw() {
    ImGui::Begin("Wavetables");

    push_tertiary_style();
    draw_add_or_remove();
    prepare_combo(wavetable_names, "##WavetableCombo", wavetable_index);
    if (ImGui::IsItemHovered()) {
        std::vector<size_t> dependencies = song.find_wavetable_dependencies(wavetable_index);
        show_dependency_tooltip("oscillators", dependencies);
    }
    pop_tertiary_style();

    ImGui::Separator();

    if (wavetables.empty()) {
        ImGui::Text("No wavetables available.");
        ImGui::End();
        return;
    }

    from();
    draw_waveform();
    check_keyboard_input();
    to();

    ImGui::End();
}

bool GUIWavetablesPanel::is_index_valid() const {
    return wavetable_index >= 0 && wavetable_index < wavetables.size();
}

float GUIWavetablesPanel::cast_to_float(uint8_t value) const {
    return (2.0f * value) / UINT8_MAX - 1.0f;
}

uint8_t GUIWavetablesPanel::cast_to_int(float value) const {
    return static_cast<uint8_t>(std::round((value + 1.0f) * UINT8_MAX / 2.0f));
}

void GUIWavetablesPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    const Wavetable *wavetable = wavetables[wavetable_index];
    current_wavetable.size = wavetable->wavetable_size;

    if (current_wavetable.wave.size() < current_wavetable.size) {
        for (size_t i = current_wavetable.wave.size(); i < current_wavetable.size; ++i) {
            const float buffer_value = cast_to_float(gui.wavetables_buffer[wavetable_index][i]);
            current_wavetable.wave.push_back(buffer_value);
        }
    } else if (current_wavetable.wave.size() > current_wavetable.size) {
        current_wavetable.wave.resize(std::min(current_wavetable.size, GUI_MAX_WAVETABLE_POINTS));
    }

    for (size_t i = 0; i < current_wavetable.wave.size(); ++i) {
        current_wavetable.wave[i] = cast_to_float(wavetable->data[i]);
    }
}

void GUIWavetablesPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    Wavetable *wavetable = wavetables[wavetable_index];
    const size_t size = current_wavetable.size;
    wavetable->wavetable_size = size;
    for (size_t i = 0; i < size; ++i) {
        const uint8_t value = cast_to_int(current_wavetable.wave[i]);
        wavetable->data[i] = value;
        gui.wavetables_buffer[wavetable_index][i] = value;
    }

    if (wavetable->wavetable_size > wavetable->wavetable_size) {
        for (size_t i = wavetable->wavetable_size; i < wavetable->wavetable_size; ++i) {
            wavetable->data[i] = gui.wavetables_buffer[wavetable_index][i];
        }
    }
}

void GUIWavetablesPanel::add() {
    Wavetable *new_wavetable = song.add_wavetable();
    if (new_wavetable == nullptr) {
        return;
    }

    wavetable_index = wavetables.size() - 1;
    update();
}

void GUIWavetablesPanel::duplicate() {
    Wavetable *new_wavetable = song.duplicate_wavetable(wavetable_index);
    if (new_wavetable == nullptr) {
        return;
    }

    wavetable_index = wavetables.size() - 1;
    update();
}

void GUIWavetablesPanel::remove() {
    if (is_index_valid()) {
        song.remove_wavetable(wavetable_index);
        wavetable_index = std::max(0, wavetable_index - 1);
        update();
    }
}

void GUIWavetablesPanel::draw_wavetable_length() {
    const size_t old_size = current_wavetable.size;
    draw_number_of_items("Points", "##WavetableLength", current_wavetable.size, 1, GUI_MAX_WAVETABLE_POINTS);

    if (old_size != current_wavetable.size) {
        current_wavetable.wave.resize(current_wavetable.size);
        if (current_wavetable.size > old_size) {
            for (int i = old_size; i < current_wavetable.size; i++) {
                current_wavetable.wave[i] = cast_to_float(gui.wavetables_buffer[wavetable_index][i]);
            }
        }
    }
}

void GUIWavetablesPanel::draw_waveform() {
    if (wavetables.empty()) {
        ImGui::Text("No wavetables available.");
        return;
    }

    ImGui::Checkbox("Show interpolation", &current_wavetable.interpolation);

    ImGui::Text("Waveform:");
    draw_wavetable_length();

    if (current_wavetable.wave.empty()) {
        ImGui::Text("No data to display.");
        return;
    }

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
        if (current_wavetable.interpolation) {
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
    gui.update(GUIElement::Oscillators);
}

void GUIWavetablesPanel::check_keyboard_input() {
}

void GUIWavetablesPanel::set_index(const int index) {
    wavetable_index = clamp_index(index, wavetables.size());
};
