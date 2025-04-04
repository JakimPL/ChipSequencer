#include "patterns.hpp"

GUIPatternsPanel::GUIPatternsPanel() {
    from();
    update();
}

void GUIPatternsPanel::update() {
}

void GUIPatternsPanel::draw() {
    ImGui::Begin("Pattern Editor");
    ImGui::Columns(channels.size(), "pattern_columns");
    ImGui::Separator();

    from();
    draw_channels();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

void GUIPatternsPanel::draw_channels() {
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
        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences, order->sequences + order->order_length);
    }
}

void GUIPatternsPanel::to() const {
}

void GUIPatternsPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }
}