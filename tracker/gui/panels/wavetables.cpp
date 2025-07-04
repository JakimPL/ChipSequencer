#include <iostream>

#include "nfd.h"

#include "../../general.hpp"
#include "../../song/buffers.hpp"
#include "../../song/song.hpp"
#include "../../song/lock/registry.hpp"
#include "../../utils/math.hpp"
#include "../../utils/paths.hpp"
#include "../enums.hpp"
#include "../gui.hpp"
#include "../init.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../utils.hpp"
#include "wavetables.hpp"

GUIWavetablesPanel::GUIWavetablesPanel(const bool visible, const bool windowed)
    : GUIPanel("Wavetables", visible, windowed) {
}

GUIElement GUIWavetablesPanel::get_element() const {
    return GUIElement::Wavetables;
}

void GUIWavetablesPanel::draw() {
    ImGui::BeginDisabled(lock_registry.is_locked(Target::WAVETABLE, wavetable_index));
    draw_waveform();
    draw_status();
    ImGui::EndDisabled();
}

bool GUIWavetablesPanel::select_item() {
    std::vector<std::string> dependencies = Song::find_wavetable_dependencies(wavetable_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies);
    prepare_combo(this, wavetable_names, "##WavetableCombo", wavetable_index, {}, false, GUI_COMBO_MARGIN_RIGHT);
    show_dependency_tooltip(dependencies);
    lock_item(Target::WAVETABLE, wavetable_index);
    pop_tertiary_style();
    ImGui::Separator();

    return !wavetables.empty();
}

void GUIWavetablesPanel::empty() {
    ImGui::Text("No wavetable available.");
}

bool GUIWavetablesPanel::is_index_valid() const {
    return wavetable_index >= 0 && wavetable_index < wavetables.size();
}

float GUIWavetablesPanel::cast_to_float(uint8_t value) {
    return (2.0f * value) / UINT8_MAX - 1.0f;
}

uint8_t GUIWavetablesPanel::cast_to_int(float value) {
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
            const float buffer_value = cast_to_float(buffers.wavetables[wavetable_index][i]);
            current_wavetable.wave.push_back(buffer_value);
        }
    } else if (current_wavetable.wave.size() > current_wavetable.size) {
        current_wavetable.wave.resize(std::min(current_wavetable.size, MAX_WAVETABLE_SIZE));
    }

    for (size_t i = 0; i < current_wavetable.wave.size(); ++i) {
        current_wavetable.wave[i] = cast_to_float(wavetable->data[i]);
    }
}

void GUIWavetablesPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    if (!save && !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    Wavetable *wavetable = wavetables[wavetable_index];
    const size_t size = current_wavetable.size;
    wavetable->wavetable_size = size;
    for (size_t i = 0; i < size; ++i) {
        const uint8_t value = cast_to_int(current_wavetable.wave[i]);
        wavetable->data[i] = value;
        buffers.wavetables[wavetable_index][i] = value;
    }
}

void GUIWavetablesPanel::add() {
    Wavetable *new_wavetable = Song::add_wavetable();
    if (new_wavetable == nullptr) {
        return;
    }

    wavetable_index = wavetables.size() - 1;
    perform_action_add(this, {Target::WAVETABLE, wavetable_index, 0});
    update();
}

void GUIWavetablesPanel::duplicate() {
    Wavetable *new_wavetable = Song::duplicate_wavetable(wavetable_index);
    if (new_wavetable == nullptr) {
        return;
    }

    wavetable_index = wavetables.size() - 1;
    update();
}

void GUIWavetablesPanel::remove() {
    if (is_index_valid()) {
        perform_action_remove(this, {Target::WAVETABLE, wavetable_index, 0}, wavetables[wavetable_index]);
        Song::remove_wavetable(wavetable_index);
        wavetable_index = std::max(0, wavetable_index - 1);
        update();
    }
}

void GUIWavetablesPanel::draw_wavetable_length() {
    const size_t old_size = current_wavetable.size;
    const LinkKey key = {Target::WAVETABLE, wavetable_index, WAVETABLE_SIZE};
    draw_number_of_items(this, "##WavetableLength", current_wavetable.size, 1, MAX_WAVETABLE_SIZE, key);

    if (old_size != current_wavetable.size) {
        current_wavetable.wave.resize(current_wavetable.size);
        if (current_wavetable.size > old_size) {
            for (int i = old_size; i < current_wavetable.size; i++) {
                current_wavetable.wave[i] = cast_to_float(buffers.wavetables[wavetable_index][i]);
            }
        }
    }
}

void GUIWavetablesPanel::draw_waveform() {
    ImGui::Checkbox("Show interpolation", &current_wavetable.interpolation);

    ImGui::Text("Waveform:");
    draw_wavetable_length();

    if (current_wavetable.wave.empty()) {
        ImGui::Text("No data to display.");
        return;
    }

    if (ImGui::Button("Save")) {
        save_wavetable_to_file();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
        load_wavetable_from_file();
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

    const ImGuiIO &io = ImGui::GetIO();
    const ImVec2 mouse_pos = io.MousePos;
    const bool is_hovered = ImGui::IsItemHovered();

    if (is_hovered) {
        const float relative_x = mouse_pos.x - canvas_p0.x;
        const float x_position = relative_x / x_step;

        if (x_position >= 0.0f && x_position < static_cast<float>(data_size)) {
            float value;
            int point_index;

            if (current_wavetable.interpolation) {
                const int index_low = static_cast<int>(std::floor(x_position));
                const int index_high = (index_low + 1) % static_cast<int>(data_size);
                const float fraction = x_position - static_cast<float>(index_low);

                const float value_low = current_wavetable.wave[index_low];
                const float value_high = current_wavetable.wave[index_high];
                value = value_low + fraction * (value_high - value_low);
                point_index = -1;
            } else {
                point_index = static_cast<int>(std::round(x_position));
                if (point_index >= static_cast<int>(data_size)) {
                    point_index = static_cast<int>(data_size) - 1;
                }
                value = current_wavetable.wave[point_index];
            }

            ImGui::BeginTooltip();
            if (point_index >= 0) {
                ImGui::Text("Point %d: %.3f", point_index, value);
            } else {
                ImGui::Text("Position %.2f: %.3f", x_position, value);
            }
            ImGui::EndTooltip();
        }
    }

    if (is_hovered && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        const float relative_x = mouse_pos.x - canvas_p0.x;
        const int index = static_cast<int>(std::round(relative_x / x_step));
        if (index >= 0 && index < static_cast<int>(data_size)) {
            const int current_point = index;
            const float relative_y = mouse_pos.y - canvas_p0.y;
            float normalized = 1.0f - (relative_y / size.y);
            normalized = clamp(normalized, 0.0f, 1.0f);
            const float value = (normalized * 2.0f) - 1.0f;
            if (value != current_wavetable.wave[current_point]) {
                const std::vector<float> old_wave = current_wavetable.wave;
                current_wavetable.wave[current_point] = value;
                const LinkKey key = {Target::WAVETABLE, wavetable_index, WAVETABLE_DATA};
                perform_action_wavetable(this, key, current_wavetable.size, current_wavetable.wave, old_wave);
            }
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

    const bool draw_points = (4 * data_size <= static_cast<size_t>(size.x));
    for (size_t i = 0; i < data_size; ++i) {
        const float x1 = canvas_p0.x + i * x_step;
        const float y1 = y_center - current_wavetable.wave[i] * (size.y / 2.0f);
        const float x2 = canvas_p0.x + (i + 1) * x_step;
        const float y2 = y_center - current_wavetable.wave[i + 1 == data_size ? 0 : i + 1] * (size.y / 2.0f);

        if (draw_points) {
            draw_list->AddCircleFilled(ImVec2(x1, y1), 3.0f, IM_COL32(0, 255, 0, 255));
        }
        if (current_wavetable.interpolation) {
            draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), IM_COL32(0, 255, 0, 255), 1.0f);
        } else {
            if (draw_points) {
                draw_list->AddCircleFilled(ImVec2(x2, y1), 3.0f, IM_COL32(0, 255, 0, 255));
            }
            draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y1), IM_COL32(0, 255, 0, 255), 1.0f);
            draw_list->AddLine(ImVec2(x2, y1), ImVec2(x2, y2), IM_COL32(0, 255, 0, 255), 1.0f);
        }
    }
}

void GUIWavetablesPanel::draw_status() {
    if (render_status.has_value()) {
        ImGui::OpenPopup(render_status.value() ? "Save success" : "Save failure");
        render_status = std::nullopt;
    }

    if (load_status.has_value()) {
        ImGui::OpenPopup(load_status.value() ? "Load success" : "Load failure");
        load_status = std::nullopt;
    }

    if (ImGui::BeginPopupModal("Save success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Sample saved successfully!");
    } else if (ImGui::BeginPopupModal("Save failure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Sample save failed!");
    } else if (ImGui::BeginPopupModal("Load success", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Sample loaded successfully!");
    } else if (ImGui::BeginPopupModal("Load failure", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        draw_popup("Sample load failed!");
    }
}

void GUIWavetablesPanel::update() {
    wavetable_index = clamp_index(wavetable_index, wavetables.size());
    update_items(wavetable_names, wavetables.size(), "Wavetable ", wavetable_index);
    gui.update(GUIElement::Oscillators);
}

void GUIWavetablesPanel::set_index(const int index) {
    wavetable_index = clamp_index(index, wavetables.size());
};

void GUIWavetablesPanel::save_wavetable_to_file() {
    nfdchar_t *target_path = nullptr;
    nfdresult_t result = NFD_SaveDialog("wav", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::unique_ptr<nfdchar_t, void (*)(void *)> path_guard(target_path, free);
        std::filesystem::path wav_path(target_path);
        wav_path = check_and_correct_path_by_extension(wav_path, ".wav");
        std::cout << "Saving sample to: " << wav_path << std::endl;
        std::vector<std::vector<float>> current_wave = prepare_wave_to_save();

        try {
            save_wave(wav_path.string(), current_wave, sample_rate, 1);
            render_status = std::filesystem::exists(wav_path);
        } catch (const std::exception &exception) {
            render_status = false;
            std::cerr << "Error saving wavetable: " << exception.what() << std::endl;
        }
    } else if (result != NFD_CANCEL) {
        render_status = false;
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

void GUIWavetablesPanel::load_wavetable_from_file() {
    nfdchar_t *target_path = nullptr;
    nfdresult_t result = NFD_OpenDialog("wav", nullptr, &target_path);
    if (result == NFD_OKAY) {
        std::unique_ptr<nfdchar_t, void (*)(void *)> path_guard(target_path, free);
        std::filesystem::path wav_path(target_path);
        std::cout << "Loading sample from: " << wav_path << std::endl;
        try {
            Samples samples = load_wave(wav_path.string());
            prepare_wave_from_load(samples);
            load_status = true;
        } catch (const std::exception &exception) {
            load_status = false;
            std::cerr << "Error loading wavetable: " << exception.what() << std::endl;
        }
    } else if (result != NFD_CANCEL) {
        load_status = false;
        std::cerr << "Error: " << NFD_GetError() << std::endl;
    }
}

std::vector<std::vector<float>> GUIWavetablesPanel::prepare_wave_to_save() const {
    std::vector<std::vector<float>> current_wave;
    current_wave.reserve(current_wavetable.size);
    for (const float value : current_wavetable.wave) {
        current_wave.push_back({value});
    }

    return current_wave;
}

void GUIWavetablesPanel::prepare_wave_from_load(Samples samples) {
    if (samples.output_channels == 0 || samples.data.empty()) {
        std::cerr << "Invalid sample." << std::endl;
        return;
    }

    const std::vector<float> old_wave = current_wavetable.wave;
    const size_t size = std::min(samples.data.size(), static_cast<size_t>(MAX_WAVETABLE_SIZE));
    current_wavetable.size = static_cast<int>(size);
    current_wavetable.wave.clear();
    current_wavetable.wave.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        current_wavetable.wave.push_back(samples.data[i][0]);
    }

    const LinkKey key = {Target::WAVETABLE, wavetable_index, WAVETABLE_DATA};
    perform_action_wavetable(this, key, current_wavetable.size, current_wavetable.wave, old_wave);
}
