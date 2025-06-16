#include "../../general.hpp"
#include "../enums.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "envelopes.hpp"

GUIEnvelopesPanel::GUIEnvelopesPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUIEnvelopesPanel::draw() {
    ImGui::Begin("Envelopes");

    if (select_item()) {
        from();
        draw_envelope();
        check_keyboard_input();
        actions();
        to();
        post_actions();
    } else {
        empty();
    }

    ImGui::End();
}

bool GUIEnvelopesPanel::select_item() {
    std::vector<std::string> dependencies = song.find_envelope_dependencies(envelope_index);
    std::vector<std::pair<ItemType, uint8_t>> link_dependencies = link_manager.find_dependencies(Target::ENVELOPE, envelope_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies, link_dependencies);
    prepare_combo(this, envelope_names, "##EnvelopeCombo", envelope_index);
    show_dependency_tooltip(dependencies);
    pop_tertiary_style();
    ImGui::Separator();

    return !envelopes.empty();
}

void GUIEnvelopesPanel::empty() {
    ImGui::Text("No envelope available.");
}

bool GUIEnvelopesPanel::is_index_valid() const {
    return envelope_index >= 0 && envelope_index < envelopes.size();
}

float GUIEnvelopesPanel::cast_to_float(int value, float scale) {
    return scale * static_cast<float>(value) / UINT16_MAX;
}

uint16_t GUIEnvelopesPanel::cast_to_int(float value, float scale) {
    return static_cast<uint16_t>(std::round(value / scale * UINT16_MAX));
}

void GUIEnvelopesPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    const Envelope *envelope = envelopes[envelope_index];
    current_envelope.base_volume = cast_to_float(envelope->base_volume, 2.0f);
    current_envelope.sustain_level = cast_to_float(envelope->sustain_level, 2.0f);
    current_envelope.attack = cast_to_float(envelope->attack, timer_constant);
    current_envelope.decay = cast_to_float(envelope->decay, timer_constant);
    current_envelope.hold = cast_to_float(envelope->hold, timer_constant);
    current_envelope.release = cast_to_float(envelope->release, timer_constant);

    gather_envelope_positions();
}

void GUIEnvelopesPanel::gather_envelope_positions() {
    current_envelope.timers.clear();
    current_envelope.timers.push_back(current_envelope.attack);
    current_envelope.timers.push_back(current_envelope.decay);
    current_envelope.timers.push_back(current_envelope.hold);
    current_envelope.timers.push_back(current_envelope.release);

    current_envelope.positions.clear();
    if (!gui.is_playing()) {
        return;
    }

    for (const Channel *channel : channels) {
        const size_t index = channel->envelope_index;
        if (index == envelope_index) {
            const uint8_t mode = envelope_mode[envelope_index];
            float timer = 0;
            for (uint8_t i = 0; i < mode; ++i) {
                timer += current_envelope.timers[i];
            }
            if (mode != PHASE_NOTE_CUT) {
                const float fraction = static_cast<float>(envelope_timer[envelope_index]) / (sample_rate << 14);
                timer += current_envelope.timers[mode] * fraction;
            }

            current_envelope.positions.push_back(timer);
        }
    }
}

void GUIEnvelopesPanel::to() const {
    if (!save &&
        (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ||
         !is_index_valid())) {
        return;
    }

    Envelope *envelope = envelopes[envelope_index];
    envelope->base_volume = cast_to_int(current_envelope.base_volume, 2.0f);
    envelope->sustain_level = cast_to_int(current_envelope.sustain_level, 2.0f);
    envelope->attack = cast_to_int(current_envelope.attack, timer_constant);
    envelope->decay = cast_to_int(current_envelope.decay, timer_constant);
    envelope->hold = cast_to_int(current_envelope.hold, timer_constant);
    envelope->release = cast_to_int(current_envelope.release, timer_constant);
}

void GUIEnvelopesPanel::add() {
    Envelope *new_envelope = song.add_envelope();
    if (new_envelope == nullptr) {
        return;
    }

    envelope_index = envelopes.size() - 1;
    update();
}

void GUIEnvelopesPanel::duplicate() {
    Envelope *new_envelope = song.duplicate_envelope(envelope_index);
    if (new_envelope == nullptr) {
        return;
    }

    envelope_index = envelopes.size() - 1;
    update();
}

void GUIEnvelopesPanel::remove() {
    if (is_index_valid()) {
        song.remove_envelope(envelope_index);
        envelope_index = std::max(0, envelope_index - 1);
        update();
    }
}

void GUIEnvelopesPanel::update() {
    update_items(envelope_names, envelopes.size(), "Envelope ", envelope_index);
    gui.update(GUIElement::Channels);
}

void GUIEnvelopesPanel::draw_envelope() {
    ImGui::Columns(2, "envelope_top_columns");
    draw_levels();
    ImGui::NextColumn();
    draw_timers();

    ImGui::Columns(1, "envelope_bottom_columns");
    draw_envelope_graph();
}

void GUIEnvelopesPanel::draw_levels() {
    ImGui::Text("Levels");
    draw_float_slider(this, "Base volume", current_envelope.base_volume, {Target::ENVELOPE, envelope_index, ENVELOPE_BASE_VOLUME}, 0.0f, 1.0f);
    draw_float_slider(this, "Sustain level", current_envelope.sustain_level, {Target::ENVELOPE, envelope_index, ENVELOPE_SUSTAIN_LEVEL}, 0.0f, 1.0f);
}

void GUIEnvelopesPanel::draw_timers() {
    ImGui::Text("Timers");
    draw_float_slider(this, "Attack", current_envelope.attack, {Target::ENVELOPE, envelope_index, ENVELOPE_ATTACK}, 0.0f, GUI_ENVELOPE_MAX_TIMER, GUIScale::Quadratic);
    draw_float_slider(this, "Decay", current_envelope.decay, {Target::ENVELOPE, envelope_index, ENVELOPE_DECAY}, 0.0f, GUI_ENVELOPE_MAX_TIMER, GUIScale::Quadratic);
    draw_float_slider(this, "Hold", current_envelope.hold, {Target::ENVELOPE, envelope_index, ENVELOPE_HOLD}, 0.0f, GUI_ENVELOPE_MAX_TIMER, GUIScale::Quadratic);
    draw_float_slider(this, "Release", current_envelope.release, {Target::ENVELOPE, envelope_index, ENVELOPE_RELEASE}, 0.0f, GUI_ENVELOPE_MAX_TIMER, GUIScale::Quadratic);
}

void GUIEnvelopesPanel::draw_envelope_graph() {
    ImGui::Separator();
    ImGui::Text("Envelope Graph");
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const float width = std::max(40.0f, ImGui::GetContentRegionAvail().x);
    const float height = std::max(20.0f, ImGui::GetContentRegionAvail().y - 20.0f);
    const ImVec2 size = ImVec2(width, height);
    ImGui::InvisibleButton("envelope_canvas", size);
    const ImVec2 canvas_p0 = ImVec2(p.x, p.y + 10.0f);
    const ImVec2 canvas_p1 = ImVec2(p.x + size.x, p.y + size.y + 10.0f);

    draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(50, 50, 50, 255));
    draw_list->AddRect(canvas_p0, canvas_p1, IM_COL32(200, 200, 200, 255));

    float total_time = current_envelope.attack + current_envelope.decay + current_envelope.hold + current_envelope.release;
    total_time = std::max(total_time, 0.001f);

    const float attack_time = current_envelope.attack / total_time;
    const float decay_time = current_envelope.decay / total_time;
    const float hold_time = current_envelope.hold / total_time;
    const float release_time = current_envelope.release / total_time;

    const ImVec2 p0 = ImVec2(canvas_p0.x, canvas_p1.y);
    const ImVec2 p1 = ImVec2(canvas_p0.x + size.x * attack_time, canvas_p0.y + (1.0f - current_envelope.base_volume) * size.y);
    const ImVec2 p2 = ImVec2(p1.x + size.x * decay_time, canvas_p1.y - current_envelope.sustain_level * height);
    const ImVec2 p3 = ImVec2(p2.x + size.x * hold_time, p2.y);
    const ImVec2 p4 = ImVec2(p3.x + size.x * release_time, canvas_p1.y);

    const float grid_step = 1.0f;
    const int grid_lines = total_time / grid_step;

    for (int i = 1; i <= grid_lines; ++i) {
        const float current_step_time = grid_step * i;
        const float x = canvas_p0.x + size.x * (current_step_time / static_cast<float>(total_time));
        const std::string label = std::to_string(static_cast<int>(grid_step * i)) + "s";
        const ImVec2 text_size = ImGui::CalcTextSize(label.c_str());
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(100, 100, 100, 255), 1.0f);
        draw_list->AddText(ImVec2(x - text_size.x / 2, canvas_p1.y + 5), IM_COL32(255, 255, 255, 255), label.c_str());
    }

    draw_list->AddLine(p0, p1, IM_COL32(255, 0, 0, 255), 2.0f);
    draw_list->AddLine(p1, p2, IM_COL32(255, 165, 0, 255), 2.0f);
    draw_list->AddLine(p2, p3, IM_COL32(75, 255, 130, 255), 2.0f);
    draw_list->AddLine(p3, p4, IM_COL32(0, 144, 255, 255), 2.0f);
    draw_list->AddCircleFilled(p1, 3.0f, IM_COL32(255, 0, 0, 255));
    draw_list->AddCircleFilled(p2, 3.0f, IM_COL32(255, 165, 0, 255));
    draw_list->AddCircleFilled(p3, 3.0f, IM_COL32(75, 255, 130, 255));
    draw_list->AddCircleFilled(p4, 3.0f, IM_COL32(0, 144, 255, 255));

    for (const float position : current_envelope.positions) {
        const float x = canvas_p0.x + size.x * (position / static_cast<float>(total_time));
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(255, 255, 0, 128), 1.0f);
    }

    const float tick_duration_base = song.get_row_duration();
    float tick_duration_seconds = tick_duration_base;
    int tick_label_multiplier = 1;
    while ((total_time / tick_duration_seconds) > GUI_ENVELOPE_MAX_TICK_DURATION_COUNT) {
        tick_duration_seconds *= 2.0f;
        tick_label_multiplier *= 2;
    }
    const int tick_count = static_cast<int>(total_time / tick_duration_seconds);

    for (int i = 1; i <= tick_count; ++i) {
        float tick_time = tick_duration_seconds * i;
        float x = canvas_p0.x + size.x * (tick_time / total_time);
        int tick_label = i * tick_label_multiplier;
        std::string tick_label_str = std::to_string(tick_label);
        ImVec2 tick_text_size = ImGui::CalcTextSize(tick_label_str.c_str());
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(0, 200, 255, 128), 1.0f);
        draw_list->AddText(ImVec2(x - tick_text_size.x / 2, canvas_p0.y - tick_text_size.y - 2), IM_COL32(0, 200, 255, 255), tick_label_str.c_str());
    }

    draw_list->AddLine(p0, p1, IM_COL32(255, 0, 0, 255), 2.0f);
    draw_list->AddLine(p1, p2, IM_COL32(255, 165, 0, 255), 2.0f);
    draw_list->AddLine(p2, p3, IM_COL32(75, 255, 130, 255), 2.0f);
    draw_list->AddLine(p3, p4, IM_COL32(0, 144, 255, 255), 2.0f);
    draw_list->AddCircleFilled(p1, 3.0f, IM_COL32(255, 0, 0, 255));
    draw_list->AddCircleFilled(p2, 3.0f, IM_COL32(255, 165, 0, 255));
    draw_list->AddCircleFilled(p3, 3.0f, IM_COL32(75, 255, 130, 255));
    draw_list->AddCircleFilled(p4, 3.0f, IM_COL32(0, 144, 255, 255));

    for (const float position : current_envelope.positions) {
        const float x = canvas_p0.x + size.x * (position / static_cast<float>(total_time));
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), IM_COL32(255, 255, 0, 128), 1.0f);
    }
}

void GUIEnvelopesPanel::set_index(const int index) {
    envelope_index = clamp_index(index, envelopes.size());
}
