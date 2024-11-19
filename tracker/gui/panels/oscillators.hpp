#ifndef GUI_PANELS_OSCILLATORS_HPP
#define GUI_PANELS_OSCILLATORS_HPP

#include <algorithm>
#include <string>
#include "../init.hpp"

class GUIOscillatorsPanel {
  private:
    struct CurrentOscillator {
        int generator_index;
        std::string type;
        float duty_cycle;
        int wavetable_index;
    } current_oscillator;

    int oscillator_index = 0;
    std::vector<std::string> oscillator_names;
    std::vector<std::string> wavetable_names;

    void from_oscillator() {
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

    void to_oscillator() {
    }

    void update_oscillators() {
        update_items(oscillator_names, oscillators.size(), "Oscillator ", oscillator_index);
    }

    void update_wavetables() {
        update_items(wavetable_names, wavetables.size(), "Wavetable ", current_oscillator.wavetable_index);
    }

    void draw_oscillator_type() {
        ImGui::Text("Type");
        ImGui::NextColumn();
        switch (current_oscillator.generator_index) {
        case OSCILLATOR_SQUARE: {
            ImGui::Text("%s", "Square");
            break;
        }
        case OSCILLATOR_SAW: {
            ImGui::Text("%s", "Saw");
            break;
        }
        case OSCILLATOR_SINE: {
            ImGui::Text("%s", "Sine");
            break;
        }
        case OSCILLATOR_WAVETABLE: {
            ImGui::Text("%s", "Wavetable");
            break;
        }
        }
        ImGui::NextColumn();
    }

    void draw_oscillator() {
        ImGui::Separator();
        ImGui::Text("Oscillator:");
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

  public:
    GUIOscillatorsPanel() {
        update_oscillators();
        update_wavetables();
    }

    void draw() {
        ImGui::Begin("Oscillator Editor");
        ImGui::Columns(1, "oscillator_columns");

        prepare_combo(oscillator_names, "##OscillatorCombo", oscillator_index);
        from_oscillator();
        draw_oscillator();

        to_oscillator();

        ImGui::Columns(1);
        ImGui::End();
    }
};

#endif // GUI_PANELS_OSCILLATORS_HPP
