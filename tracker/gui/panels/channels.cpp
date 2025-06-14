#include <iostream>

#include "../../general.hpp"
#include "../../song/links/manager.hpp"
#include "../../song/links/target.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "channels.hpp"

GUIChannelsPanel::GUIChannelsPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUIChannelsPanel::draw() {
    ImGui::Begin("Channels");
    ImGui::Columns(1, "channel_columns");

    ImGui::BeginDisabled(gui.is_playing());
    std::vector<std::pair<ItemType, uint8_t>> link_dependencies = link_manager.find_dependencies(Target::CHANNEL, channel_index);
    push_tertiary_style();
    draw_add_or_remove({}, link_dependencies);
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
    current_channel.order_index = channel->order_index;
    current_channel.oscillator_index = channel->oscillator_index;
    current_channel.output_type.from_flags(channel->output_flag, channel->flag);

    current_channel.constant_pitch = channel->flag & FLAG_CONSTANT_PITCH;
    current_channel.hide = channel->flag & FLAG_HIDDEN;
    if (current_channel.constant_pitch) {
        current_channel.sync = channel->flag & FLAG_SYNC;
        if (current_channel.sync) {
            current_channel.sync_numerator = channel->fraction & 0x0F;
            current_channel.sync_denominator = (channel->fraction >> 4) & 0x0F;

            const float fraction = static_cast<float>(current_channel.sync_denominator) / current_channel.sync_numerator;
            current_channel.pitch = song.calculate_real_bpm() / unit * fraction;
        } else {
            current_channel.pitch = static_cast<float>(channel->pitch) / 0x10000;
        }
    } else {
        current_channel.transpose = scale_composer.get_edo() * log2(static_cast<float>(channel->pitch) / DEFAULT_CHANNEL_PITCH);
    }

    const Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][channel_index];
    current_channel.output_type.from_link(link);
    current_channel.output_type.load_splitter(channel->splitter);
}

void GUIChannelsPanel::to() const {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || !is_index_valid() || gui.is_playing()) {
        return;
    }

    Channel *channel = channels[channel_index];
    channel->envelope_index = current_channel.envelope_index;
    channel->oscillator_index = current_channel.oscillator_index;
    channel->order_index = current_channel.order_index;

    channel->flag = 0;
    current_channel.output_type.set_item_flag(channel->flag);
    current_channel.output_type.set_output_flag(channel->output_flag);

    if (current_channel.hide) {
        channel->flag |= FLAG_HIDDEN;
    }

    if (current_channel.constant_pitch) {
        channel->flag |= FLAG_CONSTANT_PITCH;
        if (current_channel.sync) {
            channel->flag |= FLAG_SYNC;
            channel->fraction = (current_channel.sync_numerator & 0x0F);
            channel->fraction |= ((current_channel.sync_denominator & 0x0F) << 4);
        }

        channel->pitch = static_cast<uint32_t>(std::round(current_channel.pitch * 0x10000));
    } else {
        channel->pitch = static_cast<uint32_t>(std::round(DEFAULT_CHANNEL_PITCH * pow(2, current_channel.transpose / scale_composer.get_edo())));
    }

    Link &link = links[static_cast<size_t>(ItemType::CHANNEL)][channel_index];
    current_channel.output_type.set_link(link, ItemType::CHANNEL, channel_index);
    current_channel.output_type.set_splitter(channel->splitter);
    try {
        link_manager.set_link(link, static_cast<void *>(channel), channel_index);
        link_manager.save_targets();
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

void GUIChannelsPanel::duplicate() {
    Channel *new_channel = song.duplicate_channel(channel_index);
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

    const bool modulator = is_target_parameter(target);
    const std::string label = modulator ? "Modulator " : "Channel ";
    channel_names[index] = label + std::to_string(index);
}

void GUIChannelsPanel::draw_channel() {
    if (channels.empty()) {
        ImGui::Text("No channels available.");
        return;
    }

    ImGui::Checkbox("Bypass", &current_channel.output_type.bypass);
    ImGui::Checkbox("Hide in pattern view", &current_channel.hide);
    ImGui::Separator();

    ImGui::Text("Envelope:");
    if (prepare_combo(envelope_names, "##EnvelopeCombo", current_channel.envelope_index, true).right_clicked) {
        gui.set_index(GUIElement::Envelopes, current_channel.envelope_index);
    }

    ImGui::Text("Oscillator:");
    if (prepare_combo(oscillator_names, "##OscillatorCombo", current_channel.oscillator_index, true).right_clicked) {
        gui.set_index(GUIElement::Oscillators, current_channel.oscillator_index);
    }

    ImGui::Text("Order:");
    if (prepare_combo(order_names, "##OrderCombo", current_channel.order_index, !current_channel.constant_pitch).right_clicked) {
        gui.set_index(GUIElement::Orders, current_channel.order_index);
    }

    ImGui::Separator();
    ImGui::Text("Pitch/transpose:");
    ImGui::Checkbox("Constant pitch", &current_channel.constant_pitch);
    ImGui::BeginDisabled(!current_channel.constant_pitch);
    ImGui::Checkbox("Synchronize", &current_channel.sync);
    ImGui::EndDisabled();

    const LinkKey key = {Target::CHANNEL, channel_index, CHANNEL_PITCH};
    if (current_channel.constant_pitch) {
        if (current_channel.sync) {
            ImGui::Text("Ratio:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##Numerator", &current_channel.sync_numerator, 0, 0);
            ImGui::SameLine();
            ImGui::TextUnformatted("/");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##Denominator", &current_channel.sync_denominator, 0, 0);
            current_channel.sync_numerator = std::clamp(current_channel.sync_numerator, 1, 16);
            current_channel.sync_denominator = std::clamp(current_channel.sync_denominator, 1, 16);
        } else {
            draw_float_slider("Pitch (Hz)", current_channel.pitch, key, GUI_MIN_PITCH, GUI_MAX_PITCH, GUIScale::Logarithmic);
        }
    } else {
        draw_float_slider("Transpose", current_channel.transpose, key, GUI_MIN_TRANSPOSE, GUI_MAX_TRANSPOSE);
    }

    if (draw_output(current_channel.output_type, {Target::CHANNEL, channel_index, CHANNEL_SPLITTER})) {
        update_channel_name(channel_index, current_channel.output_type.target);
    }
}

void GUIChannelsPanel::check_keyboard_input() {
}

void GUIChannelsPanel::set_index(const int index) {
    channel_index = clamp_index(index, channels.size());
}
