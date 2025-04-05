#include "../../general.hpp"
#include "../utils.hpp"
#include "patterns.hpp"

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

    ImGui::Columns(channels.size(), "pattern_columns");
    for (const auto &[id, pattern] : current_pattern.patterns) {
        draw_channel(id);
    }
}
void GUIPatternsPanel::draw_channel(size_t channel_index) {
    ImGui::PushID(channel_index);
    ImGui::Text("Channel %zu", channel_index);
    size_t index = 0;
    for (auto &pattern : current_pattern.patterns[channel_index]) {
        const int playing_row = current_pattern.playing_rows[channel_index];
        index = draw_pattern(pattern, false, index, playing_row);
    }
    ImGui::NextColumn();
    ImGui::PopID();
}

void GUIPatternsPanel::from() {
    current_pattern.patterns.clear();
    current_pattern.playing_rows.clear();
    const bool is_playing = gui.is_playing() && ticks_per_beat > 0;
    for (size_t i = 0; i < channels.size(); ++i) {
        const Channel *channel = channels[i];
        const uint8_t order_index = channel->order_index;
        const bool constant_pitch = channel->order_index == -1;
        if (constant_pitch || order_index >= orders.size()) {
            continue;
        }

        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences, order->sequences + order->order_length);

        uint16_t row = 0;
        uint8_t playing_sequence = current_sequence[i];
        current_pattern.playing_rows[i] = -1;
        for (size_t j = 0; j < order->order_length; ++j) {
            const uint8_t sequence_index = order_sequences[j];
            if (sequence_index >= sequences.size()) {
                break;
            }

            const Sequence *sequence = sequences[sequence_index];
            Pattern pattern = Pattern(sequence);
            current_pattern.patterns[i].push_back(pattern);

            if (is_playing && playing_sequence == j) {
                const int playing_row = pattern.calculate_playing_row(i);
                current_pattern.playing_rows[i] = row + playing_row;
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
}