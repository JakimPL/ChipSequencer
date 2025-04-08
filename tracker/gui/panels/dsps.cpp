#include "../../general.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "dsps.hpp"

GUIDSPsPanel::GUIDSPsPanel() {
    from();
    update();
}

void GUIDSPsPanel::draw() {
    ImGui::Begin("DSP Editor");
    ImGui::Columns(1, "dsp_columns");

    draw_add_or_remove();
    prepare_combo(dsp_names, "##DSPCombo", dsp_index);
    ImGui::Separator();

    from();
    draw_dsp();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

bool GUIDSPsPanel::is_index_valid() const {
    return dsp_index >= 0 && dsp_index < dsps.size();
}

void GUIDSPsPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    const void *dsp = dsps[dsp_index];
    const DSP *generic = static_cast<const DSP *>(dsp);
    current_dsp.effect_index = generic->effect_index;
    switch (current_dsp.effect_index) {
    case EFFECT_GAINER: {
        current_dsp.type = "Gainer";
        const DSPGainer *gainer = static_cast<const DSPGainer *>(dsp);
        current_dsp.gainer_gain = static_cast<float>(gainer->volume) / UINT16_MAX;
        break;
    }
    case EFFECT_DELAY: {
        current_dsp.type = "Delay";
        const DSPDelay *delay = static_cast<const DSPDelay *>(dsp);
        current_dsp.delay_dry = static_cast<float>(delay->dry) / UINT16_MAX;
        current_dsp.delay_wet = static_cast<float>(delay->wet) / UINT16_MAX;
        current_dsp.delay_feedback = static_cast<float>(delay->feedback) / UINT16_MAX;
        current_dsp.delay_time = delay->delay_time;
        break;
    }
    case EFFECT_FILTER: {
        current_dsp.type = "Filter";
        const DSPFilter *filter = static_cast<const DSPFilter *>(dsp);
        current_dsp.filter_cutoff = static_cast<float>(filter->frequency) / UINT16_MAX;
        break;
    }
    }

    const Link &link = links[static_cast<size_t>(ItemType::DSP)][dsp_index];
    current_dsp.output_type.from_link(link);
}

void GUIDSPsPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    void *dsp = nullptr;
    switch (current_dsp.effect_index) {
    case EFFECT_GAINER: {
        static_cast<DSPGainer *>(dsp)->~DSPGainer();
        DSPGainer *dsp = new (dsp) DSPGainer();
        dsp->effect_index = EFFECT_GAINER;
        dsp->volume = static_cast<uint16_t>(std::round(current_dsp.gainer_gain * UINT16_MAX));
        break;
    }
    case EFFECT_DELAY: {
        static_cast<DSPDelay *>(dsp)->~DSPDelay();
        DSPDelay *dsp = new (dsp) DSPDelay();
        dsp->effect_index = EFFECT_DELAY;
        dsp->dry = static_cast<uint16_t>(std::round(current_dsp.delay_dry * UINT16_MAX));
        dsp->wet = static_cast<uint16_t>(std::round(current_dsp.delay_wet * UINT16_MAX));
        dsp->feedback = static_cast<uint16_t>(std::round(current_dsp.delay_feedback * UINT16_MAX));
        dsp->delay_time = static_cast<uint16_t>(std::round(current_dsp.delay_time));
        break;
    }
    case EFFECT_FILTER: {
        static_cast<DSPFilter *>(dsp)->~DSPFilter();
        DSPFilter *dsp = new (dsp) DSPFilter();
        dsp->effect_index = EFFECT_FILTER;
        dsp->frequency = static_cast<uint16_t>(std::round(current_dsp.filter_cutoff * UINT16_MAX));
        break;
    }
    }

    Link &link = links[static_cast<size_t>(ItemType::DSP)][dsp_index];
    current_dsp.output_type.set_link(link, ItemType::DSP, dsp_index);
    song.set_link(link, static_cast<void *>(dsp), dsp_index);
}

void GUIDSPsPanel::add() {
    void *new_dsp = song.add_dsp();
    if (new_dsp == nullptr) {
        return;
    }

    dsp_index = dsps.size() - 1;
    update();
}

void GUIDSPsPanel::remove() {
    if (is_index_valid()) {
        song.remove_dsp(dsp_index);
        dsp_index = std::max(0, dsp_index - 1);
        update();
    }
}

void GUIDSPsPanel::update() {
    update_items(dsp_names, dsps.size(), "DSP ", dsp_index);
    effect_names = {"Gainer", "Delay", "Filter"};
}

void GUIDSPsPanel::draw_dsp_type() {
    ImGui::Text("Type");
    ImGui::NextColumn();
    prepare_combo(effect_names, "##GeneratorCombo", current_dsp.effect_index);
    ImGui::NextColumn();
}

void GUIDSPsPanel::draw_dsp() {
    if (dsps.empty()) {
        ImGui::Text("No dsps available.");
        return;
    }

    draw_dsp_type();
    ImGui::NextColumn();
    draw_effect();
    ImGui::NewLine();
    draw_output(current_dsp.output_type);
}

void GUIDSPsPanel::draw_effect() {
    switch (current_dsp.effect_index) {
    case EFFECT_GAINER: {
        draw_knob("Gain", current_dsp.gainer_gain, 0.0f, 1.0f);
        break;
    }
    case EFFECT_DELAY: {
        draw_knob("Dry", current_dsp.delay_dry, 0.0f, 1.0f);
        ImGui::SameLine();
        draw_knob("Wet", current_dsp.delay_wet, 0.0f, 1.0f);
        ImGui::SameLine();
        draw_knob("Feedback", current_dsp.delay_feedback, 0.0f, 1.0f);
        ImGui::SameLine();
        draw_knob("Time", current_dsp.delay_time, 1, MAX_DSP_BUFFER_SIZE);
        break;
    }
    case EFFECT_FILTER: {
        draw_knob("Frequency", current_dsp.filter_cutoff, 0.0f, 1.0f);
        break;
    }
    }
}

void GUIDSPsPanel::check_keyboard_input() {
}
