#include "../../general.hpp"
#include "../../song/links/manager.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "dsps.hpp"

GUIDSPsPanel::GUIDSPsPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUIDSPsPanel::draw() {
    ImGui::Begin(label.c_str());
    ImGui::BeginDisabled(gui.is_playing());

    if (select_item()) {
        from();
        draw_dsp();
        check_keyboard_input();
        to();
        history_actions();
        post_actions();
    } else {
        empty();
    }

    ImGui::EndDisabled();
    ImGui::End();
}

bool GUIDSPsPanel::select_item() {
    std::vector<std::string> dependencies = song.find_dsp_dependencies(dsp_index);
    std::vector<std::pair<ItemType, uint8_t>> link_dependencies = link_manager.find_dependencies(Target::DSP, dsp_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies, link_dependencies);
    prepare_combo(this, dsp_names, "##DSPCombo", dsp_index);
    show_dependency_tooltip(dependencies);
    pop_tertiary_style();
    ImGui::Separator();

    return !dsps.empty();
}

void GUIDSPsPanel::empty() {
    ImGui::Text("No DSP available.");
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
    current_dsp.output_type.from_flags(generic->output_flag, generic->flag);
    current_dsp.effect_index = generic->effect_index;
    switch (static_cast<Effect>(current_dsp.effect_index)) {
    case Effect::Gainer: {
        current_dsp.type = "Gainer";
        const DSPGainer *gainer = static_cast<const DSPGainer *>(dsp);
        current_dsp.gainer_gain = static_cast<float>(gainer->volume) / UINT16_MAX;
        break;
    }
    case Effect::Distortion: {
        current_dsp.type = "Distortion";
        const DSPDistortion *distortion = static_cast<const DSPDistortion *>(dsp);
        current_dsp.distortion_level = 2.0f * static_cast<float>(distortion->level) / UINT16_MAX;
        break;
    }
    case Effect::Filter: {
        current_dsp.type = "Filter";
        const DSPFilter *filter = static_cast<const DSPFilter *>(dsp);
        current_dsp.filter_cutoff = static_cast<float>(filter->frequency) / UINT16_MAX;
        current_dsp.filter_mode = filter->mode >= 0x80;
        break;
    }
    case Effect::Delay: {
        current_dsp.type = "Delay";
        const DSPDelay *delay = static_cast<const DSPDelay *>(dsp);
        current_dsp.delay_dry = static_cast<float>(delay->dry) / UINT8_MAX;
        current_dsp.delay_wet = static_cast<float>(delay->wet) / UINT8_MAX;
        current_dsp.delay_feedback = static_cast<float>(delay->feedback) / UINT8_MAX;
        current_dsp.delay_time = static_cast<float>(delay->delay_time) / UINT16_MAX * 20.0f;
        break;
    }
    case Effect::Count:
    default:
        throw std::runtime_error("Unknown DSP type: " + std::to_string(current_dsp.effect_index));
    }

    const Link &link = links[static_cast<size_t>(ItemType::DSP)][dsp_index];
    current_dsp.output_type.from_link(link);
    current_dsp.output_type.load_splitter(generic->splitter);
}

void GUIDSPsPanel::to() const {
    if (!save &&
        (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ||
         !is_index_valid() ||
         gui.is_playing())) {
        return;
    }

    void *buffer = dsps[dsp_index];
    switch (static_cast<Effect>(current_dsp.effect_index)) {
    case Effect::Gainer: {
        static_cast<DSP *>(buffer)->~DSP();
        DSPGainer *dsp = new (buffer) DSPGainer();
        dsp->effect_index = EFFECT_GAINER;
        dsp->volume = static_cast<uint16_t>(std::round(current_dsp.gainer_gain * UINT16_MAX));
        break;
    }
    case Effect::Distortion: {
        static_cast<DSP *>(buffer)->~DSP();
        DSPDistortion *dsp = new (buffer) DSPDistortion();
        dsp->effect_index = EFFECT_DISTORTION;
        dsp->level = static_cast<uint16_t>(std::round(current_dsp.distortion_level * UINT16_MAX / 2.0f));
        break;
    }
    case Effect::Filter: {
        static_cast<DSP *>(buffer)->~DSP();
        DSPFilter *dsp = new (buffer) DSPFilter();
        dsp->effect_index = EFFECT_FILTER;
        dsp->frequency = static_cast<uint16_t>(std::round(current_dsp.filter_cutoff * UINT16_MAX));
        dsp->mode = current_dsp.filter_mode ? 0xFF : 0x00;
        break;
    }
    case Effect::Delay: {
        static_cast<DSP *>(buffer)->~DSP();
        DSPDelay *dsp = new (buffer) DSPDelay();
        dsp->effect_index = EFFECT_DELAY;
        dsp->dry = static_cast<uint8_t>(std::round(current_dsp.delay_dry * UINT8_MAX));
        dsp->wet = static_cast<uint8_t>(std::round(current_dsp.delay_wet * UINT8_MAX));
        dsp->feedback = static_cast<uint8_t>(std::round(current_dsp.delay_feedback * UINT8_MAX));
        dsp->delay_time = static_cast<uint16_t>(std::round(current_dsp.delay_time * UINT16_MAX / 20.0f));
        break;
    }
    case Effect::Count:
    default:
        throw std::runtime_error("Unknown DSP type: " + std::to_string(current_dsp.effect_index));
    }

    DSP *dsp = static_cast<DSP *>(buffer);
    Link &link = links[static_cast<size_t>(ItemType::DSP)][dsp_index];
    current_dsp.output_type.set_link(link, ItemType::DSP, dsp_index);
    current_dsp.output_type.set_splitter(dsp->splitter);
    current_dsp.output_type.set_output_flag(dsp->output_flag);
    current_dsp.output_type.set_item_flag(dsp->flag);
    link_manager.set_link(link, buffer, dsp_index);
    link_manager.save_targets();
}

void GUIDSPsPanel::add() {
    void *new_dsp = song.add_dsp();
    if (new_dsp == nullptr) {
        return;
    }

    dsp_index = dsps.size() - 1;
    update();
}

void GUIDSPsPanel::duplicate() {
    void *new_dsp = song.duplicate_dsp(dsp_index);
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
    update_dsp_names();
}

void GUIDSPsPanel::update_dsp_names() {
    dsp_names.resize(dsps.size());
    for (size_t i = 0; i < dsps.size(); ++i) {
        update_dsp_name(i);
    }
    if (dsp_index >= static_cast<int>(dsp_names.size())) {
        dsp_index = static_cast<int>(dsp_names.size()) - 1;
    }
    if (dsp_index < 0 && !dsp_names.empty()) {
        dsp_index = 0;
    }
}

void GUIDSPsPanel::update_dsp_name(const int index, const int effect_index) const {
    if (index < 0 || index >= static_cast<int>(dsp_names.size())) {
        return;
    }

    const DSP *dsp = static_cast<const DSP *>(dsps[index]);
    const int effect = effect_index == -1 ? dsp->effect_index : effect_index;
    dsp_names[index] = effect_names[effect] + " " + std::to_string(index);
}

void GUIDSPsPanel::draw_dsp_type() {
    ImGui::Text("Type");
    ImGui::NextColumn();

    if (prepare_combo(this, effect_names, "##GeneratorCombo", current_dsp.effect_index, {Target::DSP, dsp_index, DSP_EFFECT_INDEX}).value_changed) {
        update_dsp_name(dsp_index, current_dsp.effect_index);
    }

    ImGui::NextColumn();
}

void GUIDSPsPanel::draw_dsp() {
    draw_dsp_type();
    ImGui::NextColumn();
    draw_effect();
    ImGui::NewLine();

    draw_output(this, current_dsp.output_type, {Target::DSP, dsp_index, DSP_SPLITTER});
}

void GUIDSPsPanel::draw_effect() {
    draw_checkbox(this, "Bypass", current_dsp.output_type.bypass, {Target::SPECIAL, dsp_index, SPECIAL_DSP_BYPASS});
    ImGui::Separator();

    switch (static_cast<Effect>(current_dsp.effect_index)) {
    case Effect::Gainer: {
        draw_knob(this, "Gain", current_dsp.gainer_gain, {Target::DSP, dsp_index, DSP_GAINER_VOLUME}, 0.0f, 1.0f);
        break;
    }
    case Effect::Distortion: {
        draw_knob(this, "Level", current_dsp.distortion_level, {Target::DSP, dsp_index, DSP_DISTORTION_LEVEL}, 0.0f, 1.0f);
        ImGui::SameLine();
        break;
    }
    case Effect::Filter: {
        draw_checkbox(this, "High-pass", current_dsp.filter_mode, {Target::DSP, dsp_index, DSP_FILTER_MODE});
        ImGui::NewLine();
        draw_knob(this, "Frequency", current_dsp.filter_cutoff, {Target::DSP, dsp_index, DSP_FILTER_FREQUENCY}, 0.0f, 1.0f);
        break;
    }
    case Effect::Delay: {
        draw_knob(this, "Dry", current_dsp.delay_dry, {Target::DSP, dsp_index, DSP_DELAY_DRY}, 0.0f, 1.0f);
        ImGui::SameLine();
        draw_knob(this, "Wet", current_dsp.delay_wet, {Target::DSP, dsp_index, DSP_DELAY_WET}, 0.0f, 1.0f);
        ImGui::SameLine();
        draw_knob(this, "Feedback", current_dsp.delay_feedback, {Target::DSP, dsp_index, DSP_DELAY_FEEDBACK}, 0.0f, 1.0f);
        ImGui::SameLine();
        draw_knob(this, "Time", current_dsp.delay_time, {Target::DSP, dsp_index, DSP_DELAY_TIME}, 0.01f, 10.0f);
        break;
    }
    case Effect::Count:
    default: {
        throw std::runtime_error("Unknown DSP type: " + std::to_string(current_dsp.effect_index));
    }
    }
}

void GUIDSPsPanel::set_index(const int index) {
    dsp_index = clamp_index(index, dsps.size());
}
