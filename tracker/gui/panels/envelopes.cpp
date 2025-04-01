#include "../../general.hpp"
#include "envelopes.hpp"

GUIEnvelopesPanel::GUIEnvelopesPanel() {
    from();
    update();
}

void GUIEnvelopesPanel::draw() {
    ImGui::Begin("Envelope Editor");

    draw_add_or_remove();
    prepare_combo(envelope_names, "##EnvelopeCombo", envelope_index);
    ImGui::Separator();

    if (envelopes.empty()) {
        ImGui::Text("No envelopes available.");
        ImGui::End();
        return;
    }

    from();

    ImGui::Columns(2, "envelope_top_columns");
    draw_levels();
    ImGui::NextColumn();
    draw_timers();

    ImGui::Columns(1, "envelope_bottom_columns");
    draw_envelope_graph();

    to();

    ImGui::End();
}

bool GUIEnvelopesPanel::is_index_valid() const {
    return envelope_index >= 0 && envelope_index < envelopes.size();
}

float GUIEnvelopesPanel::cast_to_float(int value, float scale) {
    return scale * static_cast<float>(value) / (0.5 * UINT16_MAX);
}

uint16_t GUIEnvelopesPanel::cast_to_int(float value, float scale) {
    return static_cast<uint16_t>(value / scale * (0.5 * UINT16_MAX));
}

void GUIEnvelopesPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    const Envelope *envelope = envelopes[envelope_index];
    current_envelope.base_volume = cast_to_float(envelope->base_volume);
    current_envelope.sustain_level = cast_to_float(envelope->sustain_level);
    current_envelope.attack = cast_to_float(envelope->attack, timer_constant);
    current_envelope.decay = cast_to_float(envelope->decay, timer_constant);
    current_envelope.hold = cast_to_float(envelope->hold, timer_constant);
    current_envelope.release = cast_to_float(envelope->release, timer_constant);
}

void GUIEnvelopesPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    Envelope *envelope = envelopes[envelope_index];
    envelope->base_volume = cast_to_int(current_envelope.base_volume);
    envelope->sustain_level = cast_to_int(current_envelope.sustain_level);
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

void GUIEnvelopesPanel::remove() {
    if (is_index_valid()) {
        song.remove_envelope(envelope_index);
        envelope_index = std::max(0, envelope_index - 1);
        update();
    }
}

void GUIEnvelopesPanel::update() {
    update_items(envelope_names, envelopes.size(), "Envelope ", envelope_index);
    gui.channels_panel.update();
}

void GUIEnvelopesPanel::draw_levels() {
    ImGui::Text("Levels");
    draw_float_slider("Base Volume", current_envelope.base_volume, 0.0f, 1.0f);
    draw_float_slider("Sustain Level", current_envelope.sustain_level, 0.0f, 1.0f);
}

void GUIEnvelopesPanel::draw_timers() {
    ImGui::Text("Timers");
    draw_float_slider("Attack", current_envelope.attack, 0.0f, max_timer_value);
    draw_float_slider("Decay", current_envelope.decay, 0.0f, max_timer_value);
    draw_float_slider("Hold", current_envelope.hold, 0.0f, max_timer_value);
    draw_float_slider("Release", current_envelope.release, 0.0f, max_timer_value);
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
}

void GUIEnvelopesPanel::check_keyboard_input() {
}
