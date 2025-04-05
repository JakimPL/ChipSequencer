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

    ImGui::End();
}

void GUIPatternsPanel::draw_channels() {
    if (channels.empty()) {
        ImGui::Text("No channels available.");
        return;
    }

    ImGui::Columns(channels.size(), "pattern_columns");
    for (size_t i = 0; i < channels.size(); ++i) {
        draw_channel(i);
    }
}
void GUIPatternsPanel::draw_channel(size_t channel_index) {
    ImGui::PushID(channel_index);
    ImGui::Text("Channel %zu", channel_index);
    ImGui::NextColumn();
    ImGui::PopID();
}

void GUIPatternsPanel::from() {
    for (size_t i = 0; i < channels.size(); ++i) {
        const Channel *channel = channels[i];
        const uint8_t order_index = channel->order_index;
        const bool constant_pitch = channel->order_index == -1;
        if (constant_pitch || order_index >= orders.size()) {
            continue;
        }

        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences, order->sequences + order->order_length);
        for (size_t j = 0; j < order->order_length; ++j) {
            if (order_sequences[j] >= sequences.size()) {
                break;
            }

            const Sequence *sequence = sequences[order_sequences[j]];
            Pattern pattern = Pattern(sequence);
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