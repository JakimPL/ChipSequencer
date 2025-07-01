#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "../../general.hpp"
#include "../../song/song.hpp"
#include "../../song/lock/registry.hpp"
#include "../enums.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../utils.hpp"
#include "oscillators.hpp"

GUIOscillatorsPanel::GUIOscillatorsPanel(const bool visible, const bool windowed)
    : GUIPanel("Oscillators", visible, windowed) {
}

GUIElement GUIOscillatorsPanel::get_element() const {
    return GUIElement::Oscillators;
}

void GUIOscillatorsPanel::draw() {
    ImGui::BeginDisabled(lock_registry.is_locked(Target::OSCILLATOR, oscillator_index));
    draw_oscillator();
    ImGui::EndDisabled();
}

bool GUIOscillatorsPanel::select_item() {
    std::vector<std::string> dependencies = song.find_oscillator_dependencies(oscillator_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies);
    prepare_combo(this, oscillator_names, "##OscillatorCombo", oscillator_index, {}, false, GUI_COMBO_MARGIN_RIGHT);
    show_dependency_tooltip(dependencies);
    lock_item(Target::OSCILLATOR, oscillator_index);
    pop_tertiary_style();
    ImGui::Separator();

    return !oscillators.empty();
}

void GUIOscillatorsPanel::empty() {
    ImGui::Text("No oscillator available.");
}

bool GUIOscillatorsPanel::is_index_valid() const {
    return oscillator_index >= 0 && oscillator_index < oscillators.size();
}

void GUIOscillatorsPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    void *oscillator = oscillators[oscillator_index];
    const Oscillator *generic = static_cast<const Oscillator *>(oscillator);
    current_oscillator.generator_index = generic->generator_index;
    switch (static_cast<Generator>(current_oscillator.generator_index)) {
    case Generator::Square: {
        current_oscillator.type = "Square";
        const OscillatorSquare *square = static_cast<OscillatorSquare *>(oscillator);
        current_oscillator.square_duty_cycle = static_cast<float>(square->duty_cycle) / UINT16_MAX;
        break;
    }
    case Generator::Saw: {
        current_oscillator.type = "Saw";
        const OscillatorSaw *saw = static_cast<OscillatorSaw *>(oscillator);
        current_oscillator.saw_reverse = saw->reverse;
        break;
    }
    case Generator::Sine: {
        current_oscillator.type = "Sine";
        break;
    }
    case Generator::Wavetable: {
        current_oscillator.type = "Wavetable";
        const OscillatorWavetable *wavetable = static_cast<OscillatorWavetable *>(oscillator);
        current_oscillator.wavetable_index = wavetable->wavetable_index;
        current_oscillator.wavetable_interpolation = wavetable->interpolation >= 0x80;
        break;
    }
    case Generator::Noise: {
        current_oscillator.type = "Noise";
        break;
    }
    case Generator::Count:
    default: {
        throw std::runtime_error("Unknown oscillator type: " + std::to_string(generic->generator_index));
    }
    }
}

void GUIOscillatorsPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    if (!save && !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    void *buffer = oscillators[oscillator_index];
    switch (static_cast<Generator>(current_oscillator.generator_index)) {
    case Generator::Square: {
        OscillatorSquare *square = reinterpret_cast<OscillatorSquare *>(buffer);
        square->generator_index = GENERATOR_SQUARE;
        square->oscillator_size = SIZE_OSCILLATOR_SQUARE;
        square->duty_cycle = static_cast<uint16_t>(std::round(current_oscillator.square_duty_cycle * UINT16_MAX));
        break;
    }
    case Generator::Saw: {
        OscillatorSaw *saw = reinterpret_cast<OscillatorSaw *>(buffer);
        saw->generator_index = GENERATOR_SAW;
        saw->oscillator_size = SIZE_OSCILLATOR_SAW;
        saw->reverse = current_oscillator.saw_reverse;
        break;
    }
    case Generator::Sine: {
        OscillatorSine *sine = reinterpret_cast<OscillatorSine *>(buffer);
        sine->generator_index = GENERATOR_SINE;
        sine->oscillator_size = SIZE_OSCILLATOR_SINE;
        break;
    }
    case Generator::Wavetable: {
        OscillatorWavetable *wavetable = reinterpret_cast<OscillatorWavetable *>(buffer);
        wavetable->generator_index = GENERATOR_WAVETABLE;
        wavetable->oscillator_size = SIZE_OSCILLATOR_WAVETABLE;
        wavetable->wavetable_index = current_oscillator.wavetable_index;
        wavetable->interpolation = current_oscillator.wavetable_interpolation ? 0xFF : 0x00;
        break;
    }
    case Generator::Noise: {
        OscillatorNoise *noise = reinterpret_cast<OscillatorNoise *>(buffer);
        noise->generator_index = GENERATOR_NOISE;
        noise->oscillator_size = SIZE_OSCILLATOR_NOISE;
        break;
    }
    case Generator::Count:
    default: {
        throw std::runtime_error("Unknown oscillator type: " + std::to_string(current_oscillator.generator_index));
    }
    }
}

void GUIOscillatorsPanel::add() {
    void *new_oscillator = song.add_oscillator();
    if (new_oscillator == nullptr) {
        return;
    }

    oscillator_index = oscillators.size() - 1;
    perform_action_add(this, {Target::OSCILLATOR, oscillator_index, 0});
    update();
}

void GUIOscillatorsPanel::duplicate() {
    void *new_oscillator = song.duplicate_oscillator(oscillator_index);
    if (new_oscillator == nullptr) {
        return;
    }

    oscillator_index = oscillators.size() - 1;
    update();
}

void GUIOscillatorsPanel::remove() {
    if (is_index_valid()) {
        perform_action_remove(this, {Target::OSCILLATOR, oscillator_index, 0}, static_cast<Oscillator *>(oscillators[oscillator_index]));
        song.remove_oscillator(oscillator_index);
        oscillator_index = std::max(0, oscillator_index - 1);
        update();
    }
}

void GUIOscillatorsPanel::update() {
    oscillator_index = clamp_index(oscillator_index, oscillators.size());
    update_oscillator_names();
    update_wavetables();
    gui.update(GUIElement::Channels);
}

void GUIOscillatorsPanel::update_oscillator_names() {
    oscillator_names.resize(oscillators.size());
    for (size_t i = 0; i < oscillators.size(); ++i) {
        update_oscillator_name(i);
    }
    if (oscillator_index >= static_cast<int>(oscillator_names.size())) {
        oscillator_index = static_cast<int>(oscillator_names.size()) - 1;
    }
    if (oscillator_index < 0 && !oscillator_names.empty()) {
        oscillator_index = 0;
    }
}

void GUIOscillatorsPanel::update_oscillator_name(const int index, const int generator_index) {
    if (index < 0 || index >= static_cast<int>(oscillator_names.size())) {
        return;
    }

    const Oscillator *oscillator = static_cast<const Oscillator *>(oscillators[index]);
    const int generator = generator_index == -1 ? oscillator->generator_index : generator_index;
    oscillator_names[index] = generator_names[generator] + " " + std::to_string(index);
}

void GUIOscillatorsPanel::update_wavetables() {
    update_items(wavetable_names, wavetables.size(), "Wavetable ", current_oscillator.wavetable_index);
}

void GUIOscillatorsPanel::draw_oscillator_type() {
    ImGui::Text("Type");
    ImGui::NextColumn();

    if (prepare_combo(this, generator_names, "##GeneratorCombo", current_oscillator.generator_index, {Target::OSCILLATOR, oscillator_index, OSCILLATOR_GENERATOR_INDEX}).value_changed) {
        update_oscillator_name(oscillator_index, current_oscillator.generator_index);
        if (current_oscillator.generator_index == GENERATOR_WAVETABLE && wavetables.empty()) {
            song.add_wavetable();
            gui.update(GUIElement::Wavetables);
            current_oscillator.wavetable_index = wavetable_names.size() - 1;
            perform_action_add(this, {Target::WAVETABLE, current_oscillator.wavetable_index, 0});
        }
    }

    ImGui::NextColumn();
}

void GUIOscillatorsPanel::draw_oscillator() {
    ImGui::Text("Oscillator:");
    ImGui::Columns(2, "oscillator_columns", false);
    ImGui::SetColumnWidth(0, 150.0f);
    draw_oscillator_type();

    switch (static_cast<Generator>(current_oscillator.generator_index)) {
    case Generator::Sine:
    case Generator::Noise:
        break;
    case Generator::Square: {
        ImGui::Text("Duty cycle");
        ImGui::NextColumn();
        draw_float_slider(this, "##DutyCycle", current_oscillator.square_duty_cycle, {Target::OSCILLATOR, oscillator_index, OSCILLATOR_SQUARE_DUTY_CYCLE}, 0.0f, 1.0f, GUIScale::Linear, "%.6f");
        break;
    }
    case Generator::Saw: {
        draw_checkbox(this, "Reverse", current_oscillator.saw_reverse, {Target::OSCILLATOR, oscillator_index, OSCILLATOR_SAW_REVERSE});
        break;
    }
    case Generator::Wavetable: {
        ImGui::Text("Wavetable");
        ImGui::NextColumn();
        if (prepare_combo(this, wavetable_names, "##WavetableCombo", current_oscillator.wavetable_index, {Target::OSCILLATOR, oscillator_index, OSCILLATOR_WAVETABLE_WAVETABLE_INDEX}, true).right_clicked) {
            gui.set_index(GUIElement::Wavetables, current_oscillator.wavetable_index);
        }

        ImGui::NextColumn();
        draw_checkbox(this, "Interpolation", current_oscillator.wavetable_interpolation, {Target::OSCILLATOR, oscillator_index, OSCILLATOR_WAVETABLE_INTERPOLATION});
        break;
    }
    case Generator::Count:
    default: {
        throw std::runtime_error("Invalid oscillator type: " + std::to_string(current_oscillator.generator_index));
    }
    }

    ImGui::Columns(1);
}

void GUIOscillatorsPanel::set_index(const int index) {
    oscillator_index = clamp_index(index, oscillators.size());
}
