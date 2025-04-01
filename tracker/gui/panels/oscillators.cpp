#include <algorithm>
#include <cmath>

#include "../../general.hpp"
#include "oscillators.hpp"

GUIOscillatorsPanel::GUIOscillatorsPanel() {
    from();
    update();
}

void GUIOscillatorsPanel::draw() {
    ImGui::Begin("Oscillator Editor");
    ImGui::Columns(1, "oscillator_columns");

    draw_add_or_remove();
    prepare_combo(oscillator_names, "##OscillatorCombo", oscillator_index);
    ImGui::Separator();

    from();
    draw_oscillator();
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
    const Oscillator *generic = static_cast<Oscillator *>(oscillator);
    current_oscillator.generator_index = generic->generator_index;
    switch (current_oscillator.generator_index) {
    case GENERATOR_SQUARE: {
        current_oscillator.type = "Square";
        const OscillatorSquare *square = static_cast<OscillatorSquare *>(oscillator);
        current_oscillator.duty_cycle = static_cast<float>(square->duty_cycle) / UINT8_MAX;
        break;
    }
    case GENERATOR_SAW: {
        current_oscillator.type = "Saw";
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
        break;
    }
    }
}

void GUIOscillatorsPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    switch (current_oscillator.generator_index) {
    case GENERATOR_SQUARE: {
        OscillatorSquare *new_oscillator = static_cast<OscillatorSquare *>(operator new(sizeof(OscillatorSquare)));
        new_oscillator->generator_index = GENERATOR_SQUARE;
        new_oscillator->oscillator_size = SIZE_OSCILLATOR_SQUARE;
        new_oscillator->duty_cycle = static_cast<uint8_t>(std::round(current_oscillator.duty_cycle * UINT8_MAX));
        oscillators[oscillator_index] = new_oscillator;
        break;
    }
    case GENERATOR_SAW: {
        OscillatorSaw *new_oscillator = static_cast<OscillatorSaw *>(operator new(sizeof(OscillatorSaw)));
        new_oscillator->generator_index = GENERATOR_SAW;
        new_oscillator->oscillator_size = SIZE_OSCILLATOR_SAW;
        oscillators[oscillator_index] = new_oscillator;
        break;
    }
    case GENERATOR_SINE: {
        OscillatorSine *new_oscillator = static_cast<OscillatorSine *>(operator new(sizeof(OscillatorSine)));
        new_oscillator->generator_index = GENERATOR_SINE;
        new_oscillator->oscillator_size = SIZE_OSCILLATOR_SINE;
        oscillators[oscillator_index] = new_oscillator;
        break;
    }
    case GENERATOR_WAVETABLE: {
        OscillatorWavetable *new_oscillator = static_cast<OscillatorWavetable *>(operator new(sizeof(OscillatorWavetable)));
        new_oscillator->generator_index = GENERATOR_WAVETABLE;
        new_oscillator->oscillator_size = SIZE_OSCILLATOR_WAVETABLE;
        new_oscillator->wavetable_index = current_oscillator.wavetable_index;
        oscillators[oscillator_index] = new_oscillator;
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

void GUIOscillatorsPanel::remove() {
    if (is_index_valid()) {
        song.remove_oscillator(oscillator_index);
        oscillator_index = std::max(0, oscillator_index - 1);
        update();
    }
}

void GUIOscillatorsPanel::update() {
    update_oscillators();
    update_wavetables();
    gui.channels_panel.update();
}

void GUIOscillatorsPanel::update_oscillators() {
    update_items(oscillator_names, oscillators.size(), "Oscillator ", oscillator_index);
    generator_names = {"Square", "Saw", "Sine", "Wavetable"};
}

void GUIOscillatorsPanel::update_wavetables() {
    update_items(wavetable_names, wavetables.size(), "Wavetable ", current_oscillator.wavetable_index);
}

void GUIOscillatorsPanel::draw_oscillator_type() {
    ImGui::Text("Type");
    ImGui::NextColumn();
    prepare_combo(generator_names, "##GeneratorCombo", current_oscillator.generator_index);
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
        draw_float_slider("##DutyCycle", current_oscillator.duty_cycle, 0.0f, 1.0f);
        break;
    case GENERATOR_WAVETABLE:
        ImGui::Text("Wavetable");
        ImGui::NextColumn();
        prepare_combo(wavetable_names, "##WavetableCombo", current_oscillator.wavetable_index);
        break;
    }

    ImGui::Columns(1);
}
