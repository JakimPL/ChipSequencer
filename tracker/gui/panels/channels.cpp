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

    push_tertiary_style();
    draw_add_or_remove();
    prepare_combo(channel_names, "##ChannelCombo", channel_index);
    pop_tertiary_style();

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
    current_channel.constant_pitch = channel->order_index == CONSTANT_PITCH;
    current_channel.order_index = std::max(0, static_cast<int>(channel->order_index));
    current_channel.oscillator_index = channel->oscillator_index;
    current_channel.output_type.from_output_flag(channel->output_flag);

    if (current_channel.constant_pitch) {
        current_channel.pitch = static_cast<float>(channel->pitch) / 0x10000;
    } else {
        current_channel.pitch = 12 * log2(static_cast<float>(channel->pitch) / DEFAULT_CHANNEL_PITCH);
    }

    const Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][channel_index];
    current_channel.output_type.from_link(link);
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

    Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][channel_index];
    current_channel.output_type.set_link(link, ItemType::CHANNEL, channel_index);
    song.set_link(link, static_cast<void *>(channel), channel_index);
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
    update_channel_names();
    update_items(envelope_names, envelopes.size(), "Envelope ", current_channel.envelope_index);
    update_items(order_names, orders.size(), "Order ", current_channel.order_index);
    update_items(oscillator_names, oscillators.size(), "Oscillator ", current_channel.oscillator_index);
}

void GUIChannelsPanel::update_channel_names() {
    channel_names.resize(channels.size());
    for (size_t i = 0; i < channels.size(); ++i) {
        update_channel_name(i);
    }
    if (channel_index >= static_cast<int>(channel_names.size())) {
        channel_index = static_cast<int>(channel_names.size()) - 1;
    }
    if (channel_index < 0 && !channel_names.empty()) {
        channel_index = 0;
    }
}

void GUIChannelsPanel::update_channel_name(const int index) const {
    if (index < 0 || index >= static_cast<int>(channel_names.size())) {
        return;
    }

    OutputType output_type;
    const Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][index];
    output_type.from_link(link);

    const Target target = static_cast<Target>(output_type.target);
    const bool modulator = target != Target::OUTPUT_CHANNEL &&
                           target != Target::DSP_CHANNEL;

    const std::string label = modulator ? "Modulator " : "Channel ";
    channel_names[index] = label + std::to_string(index);
}

void GUIChannelsPanel::draw_channel() {
    if (channels.empty()) {
        ImGui::Text("No channels available.");
        return;
    }

    ImGui::Text("Envelope:");
    prepare_combo(envelope_names, "##EnvelopeCombo", current_channel.envelope_index, true);
    ImGui::Text("Oscillator:");
    prepare_combo(oscillator_names, "##OscillatorCombo", current_channel.oscillator_index, true);
    if (ImGui::Checkbox("Constant Pitch", &current_channel.constant_pitch)) {
        current_channel.order_index = 0;
    }

    ImGui::SameLine();
    ImGui::BeginDisabled(current_channel.constant_pitch);
    prepare_combo(order_names, "##OrderCombo", current_channel.order_index, !current_channel.constant_pitch);
    ImGui::EndDisabled();

    if (current_channel.constant_pitch) {
        draw_float_slider("Pitch", current_channel.pitch, 0.0002f, 65535.0f, true);
    } else {
        draw_float_slider("Transpose", current_channel.pitch, GUI_MIN_TRANSPOSE, GUI_MAX_TRANSPOSE);
    }

    if (draw_output(current_channel.output_type)) {
        update_channel_name(channel_index);
    }
}

void GUIChannelsPanel::check_keyboard_input() {
}
