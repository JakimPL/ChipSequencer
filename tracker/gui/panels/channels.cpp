#include <iostream>

#include "../../general.hpp"
#include "../../song/links/manager.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "channels.hpp"

GUIChannelsPanel::GUIChannelsPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUIChannelsPanel::draw() {
    ImGui::Begin("Channel Editor");
    ImGui::Columns(1, "channel_columns");

    ImGui::BeginDisabled(gui.is_playing());
    push_tertiary_style();
    draw_add_or_remove();
    prepare_combo(channel_names, "##ChannelCombo", channel_index);
    pop_tertiary_style();

    ImGui::Separator();

    from();
    draw_channel();
    check_keyboard_input();
    to();

    ImGui::EndDisabled();
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
    current_channel.output_type.load_splitter(channel->splitter, link);
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
    current_channel.output_type.set_splitter(channel->splitter);
    try {
        link_manager.set_link(link, static_cast<void *>(channel), channel_index);
    } catch (const std::out_of_range &exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
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
    update_channel_names();
    gui.update(GUIElement::Patterns);
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

void GUIChannelsPanel::update_channel_name(const int index, const int target_id) {
    if (index < 0 || index >= static_cast<int>(channel_names.size())) {
        return;
    }

    Target target;
    if (target_id == -1) {
        OutputType output_type;
        const Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][index];
        output_type.from_link(link);
        target = static_cast<Target>(output_type.target);
    } else {
        target = static_cast<Target>(target_id);
    }

    const bool modulator = target != Target::SPLITTER &&
                           target != Target::OUTPUT_CHANNEL &&
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

    const LinkKey key = {Target::CHANNEL, channel_index, CHANNEL_PITCH};
    if (current_channel.constant_pitch) {
        draw_float_slider("Pitch", current_channel.pitch, key, 0.0002f, 65535.0f, GUIScale::Logarithmic);
    } else {
        draw_float_slider("Transpose", current_channel.pitch, key, GUI_MIN_TRANSPOSE, GUI_MAX_TRANSPOSE);
    }

    if (draw_output(current_channel.output_type)) {
        update_channel_name(channel_index, current_channel.output_type.target);
    }
}

void GUIChannelsPanel::check_keyboard_input() {
}

void GUIChannelsPanel::set_index(const int index) {
    channel_index = clamp_index(index, channels.size());
}
