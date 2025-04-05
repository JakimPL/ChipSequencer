#include "../../general.hpp"
#include "../utils.hpp"
#include "patterns.hpp"

#include <iostream>

GUIPatternsPanel::GUIPatternsPanel() {
    from();
    update();
}

void GUIPatternsPanel::update() {
}

void GUIPatternsPanel::draw() {
    ImGui::Begin("Pattern Editor");
    ImGui::Separator();

    from();
    draw_channels();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

void GUIPatternsPanel::draw_channels() {
    if (current_pattern.patterns.empty()) {
        ImGui::Text("No channels available.");
        return;
    }

    size_t columns = current_pattern.patterns.size();
    const float available = ImGui::GetContentRegionAvail().x;
    const float total_min = columns * GUI_MINIMAL_CHANNEL_COLUMN_WIDTH;
    const float child_width = (available >= total_min) ? available : total_min;
    const float column_width = (available >= total_min) ? (available / columns) : GUI_MINIMAL_CHANNEL_COLUMN_WIDTH;

    ImGui::SetNextWindowContentSize(ImVec2(std::max(available, total_min), 0));
    ImGui::BeginChild("##PatternChannels", ImVec2(available, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::Columns(current_pattern.patterns.size(), "PatternColumns", true);
    for (size_t column = 0; column < columns; column++) {
        ImGui::SetColumnWidth(column, column_width);
    }

    for (const auto &[id, pattern] : current_pattern.patterns) {
        draw_channel(id);
    }

    ImGui::EndChild();
}

void GUIPatternsPanel::draw_channel(size_t channel_index) {
    ImGui::PushID(channel_index);
    ImGui::Text("Channel %zu", channel_index);
    size_t index = 0;
    for (auto &pattern : current_pattern.patterns[channel_index]) {
        const int playing_row = current_pattern.playing_rows[channel_index];
        auto [new_index, select] = draw_pattern(pattern, false, index, playing_row);
        if (select) {
            current_channel = channel_index;
            current_row = pattern.current_row + index;
        }
        index = new_index;
    }
    ImGui::NextColumn();
    ImGui::PopID();
}

void GUIPatternsPanel::from() {
    current_pattern.patterns.clear();
    current_pattern.playing_rows.clear();
    const bool is_playing = gui.is_playing() && ticks_per_beat > 0;
    for (size_t channel_index = 0; channel_index < channels.size(); ++channel_index) {
        const Channel *channel = channels[channel_index];
        const uint8_t order_index = channel->order_index;
        const bool constant_pitch = channel->order_index == CONSTANT_PITCH;
        if (constant_pitch || order_index >= orders.size()) {
            continue;
        }

        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences, order->sequences + order->order_length);

        uint16_t row = 0;
        uint8_t playing_sequence = current_sequence[channel_index];
        current_pattern.playing_rows[channel_index] = -1;
        for (size_t j = 0; j < order->order_length; ++j) {
            const uint8_t sequence_index = order_sequences[j];
            if (sequence_index >= sequences.size()) {
                break;
            }

            const Sequence *sequence = sequences[sequence_index];
            Pattern pattern = Pattern(sequence);
            pattern.current_row = channel_index == current_channel ? current_row - row : -1;
            current_pattern.patterns[channel_index].push_back(pattern);
            if (is_playing && playing_sequence == j) {
                const int playing_row = pattern.calculate_playing_row(channel_index);
                current_pattern.playing_rows[channel_index] = row + playing_row;
            }

            row += pattern.steps;
        }
    }
}

void GUIPatternsPanel::to() const {
}

void GUIPatternsPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    Pattern *pattern = find_pattern_by_current_row();
    if (pattern != nullptr) {
        pattern->handle_input();
    }
}

Pattern *GUIPatternsPanel::find_pattern_by_current_row() {
    if (current_channel >= current_pattern.patterns.size()) {
        return nullptr;
    }

    auto &patterns = current_pattern.patterns.at(current_channel);
    uint16_t rows = 0;
    size_t pattern_id = 0;
    for (pattern_id = 0; pattern_id < patterns.size(); pattern_id++) {
        rows += patterns[pattern_id].steps;
        if (rows > current_row) {
            break;
        }
    }

    if (pattern_id >= patterns.size()) {
        return nullptr;
    }

    return &patterns.at(pattern_id);
}