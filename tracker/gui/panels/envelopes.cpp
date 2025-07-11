#include "../../general.hpp"
#include "../../song/song.hpp"
#include "../../song/links/manager.hpp"
#include "../../song/lock/registry.hpp"
#include "../enums.hpp"
#include "../keys.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../utils.hpp"
#include "../themes/theme.hpp"
#include "envelopes.hpp"

GUIEnvelopesPanel::GUIEnvelopesPanel(const bool visible, const bool windowed)
    : GUIPanel("Envelopes", visible, windowed) {
}

GUIElement GUIEnvelopesPanel::get_element() const {
    return GUIElement::Envelopes;
}

void GUIEnvelopesPanel::draw() {
    ImGui::BeginDisabled(lock_registry.is_locked(Target::ENVELOPE, envelope_index));
    draw_envelope();
    ImGui::EndDisabled();
}

bool GUIEnvelopesPanel::select_item() {
    std::vector<std::string> dependencies = Song::find_envelope_dependencies(envelope_index);
    std::vector<std::pair<ItemType, uint8_t>> link_dependencies = LinkManager::find_dependencies(Target::ENVELOPE, envelope_index);
    push_tertiary_style();

    draw_add_or_remove(dependencies, link_dependencies);
    prepare_combo(this, envelope_names, "##EnvelopeCombo", envelope_index, {}, false, GUI_COMBO_MARGIN_RIGHT);
    show_dependency_tooltip(dependencies);
    lock_item(Target::ENVELOPE, envelope_index);
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
    if (!is_index_valid()) {
        return;
    }

    if (!save && !focus) {
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
    Envelope *new_envelope = Song::add_envelope();
    if (new_envelope == nullptr) {
        return;
    }

    envelope_index = envelopes.size() - 1;
    perform_action_add(this, {Target::ENVELOPE, envelope_index, 0});
    update();
}

void GUIEnvelopesPanel::duplicate() {
    Envelope *new_envelope = Song::duplicate_envelope(envelope_index);
    if (new_envelope == nullptr) {
        return;
    }

    envelope_index = envelopes.size() - 1;
    update();
}

void GUIEnvelopesPanel::remove() {
    if (is_index_valid()) {
        perform_action_remove(this, {Target::ENVELOPE, envelope_index, 0}, envelopes[envelope_index]);
        Song::remove_envelope(envelope_index);
        envelope_index = std::max(0, envelope_index - 1);
        update();
    }
}

void GUIEnvelopesPanel::update() {
    envelope_index = clamp_index(envelope_index, envelopes.size());
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
    ImGui::Text("Envelope graph");
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    const float width = std::max(40.0f, ImGui::GetContentRegionAvail().x);
    const float height = std::max(20.0f, ImGui::GetContentRegionAvail().y - 20.0f);
    const ImVec2 size = ImVec2(width, height);
    ImGui::InvisibleButton("envelope_canvas", size);
    const ImVec2 canvas_p0 = ImVec2(p.x, p.y + 10.0f);
    const ImVec2 canvas_p1 = ImVec2(p.x + size.x, p.y + size.y + 10.0f);

    draw_list->AddRectFilled(canvas_p0, canvas_p1, theme.get_u32_color(ThemeItem::EnvelopeCanvasBackground));
    draw_list->AddRect(canvas_p0, canvas_p1, theme.get_u32_color(ThemeItem::EnvelopeCanvasBorder));

    const float tick_duration_base = Song::get_row_duration();
    float total_time = current_envelope.attack + current_envelope.decay + current_envelope.hold + current_envelope.release;
    total_time = std::max(total_time, 0.001f);

    const bool is_hovered = ImGui::IsItemHovered();
    if (is_hovered) {
        const ImGuiIO &io = ImGui::GetIO();
        const ImVec2 mouse_pos = io.MousePos;
        const float relative_x = mouse_pos.x - canvas_p0.x;
        const float time_position = (relative_x / size.x) * total_time;

        if (time_position >= 0.0f && time_position <= total_time) {
            float envelope_value = 0.0f;
            const char *phase_name = "";

            if (time_position <= current_envelope.attack) {
                const float progress = time_position / std::max(current_envelope.attack, 0.001f);
                envelope_value = progress * current_envelope.base_volume;
                phase_name = "Attack";
            } else if (time_position <= current_envelope.attack + current_envelope.decay) {
                const float decay_start_time = current_envelope.attack;
                const float progress = (time_position - decay_start_time) / std::max(current_envelope.decay, 0.001f);
                envelope_value = current_envelope.base_volume + progress * (current_envelope.sustain_level - current_envelope.base_volume);
                phase_name = "Decay";
            } else if (time_position <= current_envelope.attack + current_envelope.decay + current_envelope.hold) {
                envelope_value = current_envelope.sustain_level;
                phase_name = "Hold";
            } else {
                const float release_start_time = current_envelope.attack + current_envelope.decay + current_envelope.hold;
                const float progress = (time_position - release_start_time) / std::max(current_envelope.release, 0.001f);
                envelope_value = current_envelope.sustain_level * (1.0f - progress);
                phase_name = "Release";
            }

            const float row_number = time_position / tick_duration_base;

            ImGui::BeginTooltip();
            ImGui::Text("%s", phase_name);
            ImGui::Text("Time: %.3fs (Row %.1f)", time_position, row_number);
            ImGui::Text("Value: %.3f", envelope_value);
            ImGui::EndTooltip();
        }
    }

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
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), theme.get_u32_color(ThemeItem::EnvelopeGridLine), 1.0f);
        draw_list->AddText(ImVec2(x - text_size.x / 2, canvas_p1.y + 5), theme.get_u32_color(ThemeItem::EnvelopeGridText), label.c_str());
    }

    draw_list->AddLine(p0, p1, theme.get_u32_color(ThemeItem::EnvelopeAttack), 2.0f);
    draw_list->AddLine(p1, p2, theme.get_u32_color(ThemeItem::EnvelopeDecay), 2.0f);
    draw_list->AddLine(p2, p3, theme.get_u32_color(ThemeItem::EnvelopeSustain), 2.0f);
    draw_list->AddLine(p3, p4, theme.get_u32_color(ThemeItem::EnvelopeRelease), 2.0f);
    draw_list->AddCircleFilled(p1, 3.0f, theme.get_u32_color(ThemeItem::EnvelopeAttack));
    draw_list->AddCircleFilled(p2, 3.0f, theme.get_u32_color(ThemeItem::EnvelopeDecay));
    draw_list->AddCircleFilled(p3, 3.0f, theme.get_u32_color(ThemeItem::EnvelopeSustain));
    draw_list->AddCircleFilled(p4, 3.0f, theme.get_u32_color(ThemeItem::EnvelopeRelease));

    for (const float position : current_envelope.positions) {
        const float x = canvas_p0.x + size.x * (position / static_cast<float>(total_time));
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), theme.get_u32_color(ThemeItem::EnvelopePlaybackLine), 1.0f);
    }

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
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), theme.get_u32_color(ThemeItem::EnvelopeCurrentLine), 1.0f);
        draw_list->AddText(ImVec2(x - tick_text_size.x / 2, canvas_p0.y - tick_text_size.y - 2), theme.get_u32_color(ThemeItem::EnvelopeCurrentLine), tick_label_str.c_str());
    }

    draw_list->AddLine(p0, p1, theme.get_u32_color(ThemeItem::EnvelopeAttack), 2.0f);
    draw_list->AddLine(p1, p2, theme.get_u32_color(ThemeItem::EnvelopeDecay), 2.0f);
    draw_list->AddLine(p2, p3, theme.get_u32_color(ThemeItem::EnvelopeSustain), 2.0f);
    draw_list->AddLine(p3, p4, theme.get_u32_color(ThemeItem::EnvelopeRelease), 2.0f);
    draw_list->AddCircleFilled(p1, 3.0f, theme.get_u32_color(ThemeItem::EnvelopeAttack));
    draw_list->AddCircleFilled(p2, 3.0f, theme.get_u32_color(ThemeItem::EnvelopeDecay));
    draw_list->AddCircleFilled(p3, 3.0f, theme.get_u32_color(ThemeItem::EnvelopeSustain));
    draw_list->AddCircleFilled(p4, 3.0f, theme.get_u32_color(ThemeItem::EnvelopeRelease));

    for (const float position : current_envelope.positions) {
        const float x = canvas_p0.x + size.x * (position / static_cast<float>(total_time));
        draw_list->AddLine(ImVec2(x, canvas_p0.y), ImVec2(x, canvas_p1.y), theme.get_u32_color(ThemeItem::EnvelopePlaybackLine), 1.0f);
    }
}

void GUIEnvelopesPanel::set_index(const int index) {
    envelope_index = clamp_index(index, envelopes.size());
}
