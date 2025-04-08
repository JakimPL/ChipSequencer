#include "../../general.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "channels.hpp"

GUIChannelsPanel::GUIChannelsPanel() {
    from();
    update();
}

void GUIChannelsPanel::draw() {
    ImGui::Begin("Channel Editor");
    ImGui::Columns(1, "channel_columns");

    draw_add_or_remove();
    prepare_combo(channel_names, "##ChannelCombo", channel_index);
    ImGui::Separator();

    from();
    draw_channel();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

bool GUIChannelsPanel::is_index_valid() const {
    return channel_index >= 0 && channel_index < channels.size();
}

void GUIChannelsPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    const Channel *channel = channels[channel_index];
    current_channel.envelope_index = channel->envelope_index;
    current_channel.constant_pitch = channel->order_index == 0xFF;
    current_channel.order_index = std::max(0, static_cast<int>(channel->order_index));
    current_channel.oscillator_index = channel->oscillator_index;
    current_channel.output_type.from_output_flag(channel->output_flag);

    if (current_channel.constant_pitch) {
        current_channel.pitch = static_cast<float>(channel->pitch) / 0x10000;
    } else {
        current_channel.pitch = 12 * log2(static_cast<float>(channel->pitch) / DEFAULT_CHANNEL_PITCH);
    }
}

void GUIChannelsPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    Channel *channel = channels[channel_index];
    channel->envelope_index = current_channel.envelope_index;
    channel->oscillator_index = current_channel.oscillator_index;

    channel->output_flag = current_channel.output_type.calculate_output_flag();
    channel->order_index = current_channel.constant_pitch ? CONSTANT_PITCH : current_channel.order_index;
    if (current_channel.constant_pitch) {
        channel->pitch = static_cast<uint32_t>(std::round(current_channel.pitch * 0x10000));
    } else {
        channel->pitch = static_cast<uint32_t>(std::round(DEFAULT_CHANNEL_PITCH * pow(2, current_channel.pitch / 12)));
    }
}

void GUIChannelsPanel::add() {
    Channel *new_channel = song.add_channel();
    if (new_channel == nullptr) {
        return;
    }

    channel_index = channels.size() - 1;
    update();
}

void GUIChannelsPanel::remove() {
    if (is_index_valid()) {
        song.remove_channel(channel_index);
        channel_index = std::max(0, channel_index - 1);
        update();
    }
}

void GUIChannelsPanel::update() {
    update_items(channel_names, channels.size(), "Channel ", channel_index);
    update_items(envelope_names, envelopes.size(), "Envelope ", current_channel.envelope_index);
    update_items(order_names, orders.size(), "Order ", current_channel.order_index);
    update_items(oscillator_names, oscillators.size(), "Oscillator ", current_channel.oscillator_index);
}

void GUIChannelsPanel::draw_channel() {
    if (channels.empty()) {
        ImGui::Text("No channels available.");
        return;
    }

    ImGui::Text("Envelope:");
    prepare_combo(envelope_names, "##EnvelopeCombo", current_channel.envelope_index);
    ImGui::Text("Oscillator:");
    prepare_combo(oscillator_names, "##OscillatorCombo", current_channel.oscillator_index);
    if (ImGui::Checkbox("Constant Pitch", &current_channel.constant_pitch) && !orders.empty()) {
        current_channel.order_index = 0;
    }

    ImGui::SameLine();
    ImGui::BeginDisabled(current_channel.constant_pitch);
    prepare_combo(order_names, "##OrderCombo", current_channel.order_index);
    ImGui::EndDisabled();

    if (current_channel.constant_pitch) {
        draw_float_slider("Pitch", current_channel.pitch, 0.0002f, 65535.0f, true);
    } else {
        draw_float_slider("Transpose", current_channel.pitch, GUI_MIN_TRANSPOSE, GUI_MAX_TRANSPOSE);
    }

    draw_output(current_channel.output_type);
}

void GUIChannelsPanel::check_keyboard_input() {
}
