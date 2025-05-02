#include <algorithm>
#include <cmath>

#include "../../general.hpp"
#include "../enums.hpp"
#include "../names.hpp"
#include "../utils.hpp"

GUIOscillatorsPanel::GUIOscillatorsPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUIOscillatorsPanel::draw() {
    ImGui::Begin("Oscillators");
    ImGui::Columns(1, "oscillator_columns");

    std::vector<size_t> dependencies = song.find_oscillator_dependencies(oscillator_index);
    push_tertiary_style();
    draw_add_or_remove("channels", dependencies);
    prepare_combo(oscillator_names, "##OscillatorCombo", oscillator_index);
    show_dependency_tooltip("channels", dependencies);

    pop_tertiary_style();

    ImGui::Separator();

    from();
    draw_oscillator();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
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
    switch (current_oscillator.generator_index) {
    case GENERATOR_SQUARE: {
        current_oscillator.type = "Square";
        const OscillatorSquare *square = static_cast<OscillatorSquare *>(oscillator);
        current_oscillator.square_duty_cycle = static_cast<float>(square->duty_cycle) / UINT8_MAX;
        break;
    }
    case GENERATOR_SAW: {
        current_oscillator.type = "Saw";
        const OscillatorSaw *saw = static_cast<OscillatorSaw *>(oscillator);
        current_oscillator.saw_reverse = saw->reverse;
        break;
    }
    case GENERATOR_SINE: {
        current_oscillator.type = "Sine";
        break;
    }
    case GENERATOR_WAVETABLE: {
        current_oscillator.type = "Wavetable";
        const OscillatorWavetable *wavetable = static_cast<OscillatorWavetable *>(oscillator);
        current_oscillator.wavetable_index = wavetable->wavetable_index;
        current_oscillator.wavetable_interpolation = wavetable->interpolation >= 0x80;
        break;
    }
    case GENERATOR_NOISE: {
        current_oscillator.type = "Noise";
        break;
    }
    }
}

void GUIOscillatorsPanel::to() const {
    if (!ImGui::IsWindowFocused() || !is_index_valid()) {
        return;
    }

    void *buffer = oscillators[oscillator_index];
    switch (current_oscillator.generator_index) {
    case GENERATOR_SQUARE: {
        OscillatorSquare *square = reinterpret_cast<OscillatorSquare *>(buffer);
        square->generator_index = GENERATOR_SQUARE;
        square->oscillator_size = SIZE_OSCILLATOR_SQUARE;
        square->duty_cycle = static_cast<uint8_t>(std::round(current_oscillator.square_duty_cycle * UINT8_MAX));
        break;
    }
    case GENERATOR_SAW: {
        OscillatorSaw *saw = reinterpret_cast<OscillatorSaw *>(buffer);
        saw->generator_index = GENERATOR_SAW;
        saw->oscillator_size = SIZE_OSCILLATOR_SAW;
        saw->reverse = current_oscillator.saw_reverse;
        break;
    }
    case GENERATOR_SINE: {
        OscillatorSine *sine = reinterpret_cast<OscillatorSine *>(buffer);
        sine->generator_index = GENERATOR_SINE;
        sine->oscillator_size = SIZE_OSCILLATOR_SINE;
        break;
    }
    case GENERATOR_WAVETABLE: {
        OscillatorWavetable *wavetable = reinterpret_cast<OscillatorWavetable *>(buffer);
        wavetable->generator_index = GENERATOR_WAVETABLE;
        wavetable->oscillator_size = SIZE_OSCILLATOR_WAVETABLE;
        wavetable->wavetable_index = current_oscillator.wavetable_index;
        wavetable->interpolation = current_oscillator.wavetable_interpolation ? 0xFF : 0x00;
        break;
    }
    case GENERATOR_NOISE: {
        OscillatorNoise *noise = reinterpret_cast<OscillatorNoise *>(buffer);
        noise->generator_index = GENERATOR_NOISE;
        noise->oscillator_size = SIZE_OSCILLATOR_NOISE;
        break;
    }
    }
}

void GUIOscillatorsPanel::add() {
    void *new_oscillator = song.add_oscillator();
    if (new_oscillator == nullptr) {
        return;
    }

    oscillator_index = oscillators.size() - 1;
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
        song.remove_oscillator(oscillator_index);
        oscillator_index = std::max(0, oscillator_index - 1);
        update();
    }
}

void GUIOscillatorsPanel::update() {
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

    if (prepare_combo(generator_names, "##GeneratorCombo", current_oscillator.generator_index).value_changed) {
        update_oscillator_name(oscillator_index, current_oscillator.generator_index);
        if (current_oscillator.generator_index == GENERATOR_WAVETABLE && wavetables.empty()) {
            song.add_wavetable();
            gui.update(GUIElement::Wavetables);
            current_oscillator.wavetable_index = wavetable_names.size() - 1;
        }
    }

    ImGui::NextColumn();
}

void GUIOscillatorsPanel::draw_oscillator() {
    ImGui::Text("Oscillator:");

    if (oscillators.empty()) {
        ImGui::Text("No oscillator available");
        ImGui::Columns(1);
        return;
    }

    ImGui::Columns(2, "oscillator_columns", false);
    ImGui::SetColumnWidth(0, 150.0f);
    draw_oscillator_type();

    switch (current_oscillator.generator_index) {
    case GENERATOR_SQUARE:
        ImGui::Text("Duty Cycle");
        ImGui::NextColumn();
        draw_float_slider("##DutyCycle", current_oscillator.square_duty_cycle, {Target::OSCILLATOR, oscillator_index, OSCILLATOR_SQUARE_DUTY_CYCLE}, 0.0f, 1.0f);
        break;
    case GENERATOR_SAW:
        ImGui::Checkbox("Reverse", &current_oscillator.saw_reverse);
        break;
    case GENERATOR_WAVETABLE:
        ImGui::Text("Wavetable");
        ImGui::NextColumn();
        if (prepare_combo(wavetable_names, "##WavetableCombo", current_oscillator.wavetable_index, true).right_clicked) {
            gui.set_index(GUIElement::Wavetables, current_oscillator.wavetable_index);
        }

        ImGui::NextColumn();
        ImGui::Checkbox("Interpolation", &current_oscillator.wavetable_interpolation);
        break;
    }

    ImGui::Columns(1);
}

void GUIOscillatorsPanel::check_keyboard_input() {
}

void GUIOscillatorsPanel::set_index(const int index) {
    oscillator_index = clamp_index(index, oscillators.size());
}
