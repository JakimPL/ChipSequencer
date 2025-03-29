#include <algorithm>

#include "oscillators.hpp"

void GUIOscillatorsPanel::from_oscillator() {
    if (oscillators.empty()) {
        return;
    }

    void *oscillator = oscillators[oscillator_index];
    const Oscillator *generic = static_cast<Oscillator *>(oscillator);
    current_oscillator.generator_index = generic->generator_index;
    switch (current_oscillator.generator_index) {
    case OSCILLATOR_SQUARE: {
        current_oscillator.type = "Square";
        const OscillatorSquare *square = static_cast<OscillatorSquare *>(oscillator);
        current_oscillator.duty_cycle = static_cast<float>(square->duty_cycle) / UINT8_MAX;
        break;
    }
    case OSCILLATOR_SAW: {
        current_oscillator.type = "Saw";
        break;
    }
    case OSCILLATOR_SINE: {
        current_oscillator.type = "Sine";
        break;
    }
    case OSCILLATOR_WAVETABLE: {
        current_oscillator.type = "Wavetable";
        const OscillatorWavetable *wavetable = static_cast<OscillatorWavetable *>(oscillator);
        current_oscillator.wavetable_index = wavetable->wavetable_index;
        break;
    }
    }
}

void GUIOscillatorsPanel::to_oscillator() {
    if (oscillators.empty()) {
        return;
    }

    switch (current_oscillator.generator_index) {
    case OSCILLATOR_SQUARE: {
        OscillatorSquare *new_oscillator = static_cast<OscillatorSquare *>(operator new(sizeof(OscillatorSquare)));
        new_oscillator->generator_index = OSCILLATOR_SQUARE;
        new_oscillator->oscillator_size = OSCILLATOR_SQUARE_SIZE;
        new_oscillator->duty_cycle = static_cast<uint8_t>(std::round(current_oscillator.duty_cycle * UINT8_MAX));
        oscillators[oscillator_index] = new_oscillator;
        break;
    }
    case OSCILLATOR_SAW: {
        OscillatorSaw *new_oscillator = static_cast<OscillatorSaw *>(operator new(sizeof(OscillatorSaw)));
        new_oscillator->generator_index = OSCILLATOR_SAW;
        new_oscillator->oscillator_size = OSCILLATOR_SAW_SIZE;
        oscillators[oscillator_index] = new_oscillator;
        break;
    }
    case OSCILLATOR_SINE: {
        OscillatorSine *new_oscillator = static_cast<OscillatorSine *>(operator new(sizeof(OscillatorSine)));
        new_oscillator->generator_index = OSCILLATOR_SINE;
        new_oscillator->oscillator_size = OSCILLATOR_SINE_SIZE;
        oscillators[oscillator_index] = new_oscillator;
        break;
    }
    case OSCILLATOR_WAVETABLE: {
        OscillatorWavetable *new_oscillator = static_cast<OscillatorWavetable *>(operator new(sizeof(OscillatorWavetable)));
        new_oscillator->generator_index = OSCILLATOR_WAVETABLE;
        new_oscillator->oscillator_size = OSCILLATOR_WAVETABLE_SIZE;
        new_oscillator->wavetable_index = current_oscillator.wavetable_index;
        oscillators[oscillator_index] = new_oscillator;
        break;
    }
    }
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
    ImGui::Separator();
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
    case OSCILLATOR_SQUARE:
        ImGui::Text("Duty Cycle");
        ImGui::NextColumn();
        draw_float_slider("##DutyCycle", current_oscillator.duty_cycle, 0.0f, 1.0f);
        break;
    case OSCILLATOR_WAVETABLE:
        ImGui::Text("Wavetable");
        ImGui::NextColumn();
        prepare_combo(wavetable_names, "##WavetableCombo", current_oscillator.wavetable_index);
        break;
    }

    ImGui::Columns(1);
}

GUIOscillatorsPanel::GUIOscillatorsPanel() {
    update_oscillators();
    update_wavetables();
}

void GUIOscillatorsPanel::draw() {
    ImGui::Begin("Oscillator Editor");
    ImGui::Columns(1, "oscillator_columns");

    prepare_combo(oscillator_names, "##OscillatorCombo", oscillator_index);
    from_oscillator();
    draw_oscillator();

    to_oscillator();

    ImGui::Columns(1);
    ImGui::End();
}
