#include <algorithm>
#include <cmath>

#include "../../general.hpp"
#include "../enums.hpp"
#include "../names.hpp"
#include "../utils.hpp"

GUIOscillatorsPanel::GUIOscillatorsPanel() {
    from();
    update();
}

void GUIOscillatorsPanel::draw() {
    ImGui::Begin("Oscillator Editor");
    ImGui::Columns(1, "oscillator_columns");

    push_tertiary_style();
    draw_add_or_remove();
    prepare_combo(oscillator_names, "##OscillatorCombo", oscillator_index);
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

    void *buffer = oscillators[oscillator_index];
    switch (current_oscillator.generator_index) {
    case GENERATOR_SQUARE: {
        static_cast<OscillatorSquare *>(buffer)->~OscillatorSquare();
        OscillatorSquare *osc = new (buffer) OscillatorSquare();
        osc->generator_index = GENERATOR_SQUARE;
        osc->oscillator_size = SIZE_OSCILLATOR_SQUARE;
        osc->duty_cycle = static_cast<uint8_t>(std::round(current_oscillator.duty_cycle * UINT8_MAX));
        break;
    }
    case GENERATOR_SAW: {
        static_cast<OscillatorSaw *>(buffer)->~OscillatorSaw();
        OscillatorSaw *osc = new (buffer) OscillatorSaw();
        osc->generator_index = GENERATOR_SAW;
        osc->oscillator_size = SIZE_OSCILLATOR_SAW;
        break;
    }
    case GENERATOR_SINE: {
        static_cast<OscillatorSine *>(buffer)->~OscillatorSine();
        OscillatorSine *osc = new (buffer) OscillatorSine();
        osc->generator_index = GENERATOR_SINE;
        osc->oscillator_size = SIZE_OSCILLATOR_SINE;
        break;
    }
    case GENERATOR_WAVETABLE: {
        static_cast<OscillatorWavetable *>(buffer)->~OscillatorWavetable();
        OscillatorWavetable *osc = new (buffer) OscillatorWavetable();
        osc->generator_index = GENERATOR_WAVETABLE;
        osc->oscillator_size = SIZE_OSCILLATOR_WAVETABLE;
        osc->wavetable_index = current_oscillator.wavetable_index;
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

    std::string label;
    const Oscillator *oscillator = static_cast<const Oscillator *>(oscillators[index]);
    const int generator = generator_index == -1 ? oscillator->generator_index : generator_index;
    switch (generator) {
    case GENERATOR_SQUARE: {
        label = "Square ";
        break;
    }
    case GENERATOR_SAW: {
        label = "Saw ";
        break;
    }
    case GENERATOR_SINE: {
        label = "Sine ";
        break;
    }
    case GENERATOR_WAVETABLE: {
        label = "Wavetable ";
        break;
    }
    default: {
        label = "Oscillator ";
        break;
    }
    }

    oscillator_names[index] = label + std::to_string(index);
}

void GUIOscillatorsPanel::update_wavetables() {
    update_items(wavetable_names, wavetables.size(), "Wavetable ", current_oscillator.wavetable_index);
}

void GUIOscillatorsPanel::draw_oscillator_type() {
    ImGui::Text("Type");
    ImGui::NextColumn();

    if (prepare_combo(generator_names, "##GeneratorCombo", current_oscillator.generator_index)) {
        update_oscillator_name(oscillator_index, current_oscillator.generator_index);
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
        draw_float_slider("##DutyCycle", current_oscillator.duty_cycle, 0.0f, 1.0f);
        break;
    case GENERATOR_WAVETABLE:
        ImGui::Text("Wavetable");
        ImGui::NextColumn();
        prepare_combo(wavetable_names, "##WavetableCombo", current_oscillator.wavetable_index, true);
        break;
    }

    ImGui::Columns(1);
}

void GUIOscillatorsPanel::check_keyboard_input() {
}
