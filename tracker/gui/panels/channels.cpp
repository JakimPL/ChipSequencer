#include <iostream>

#include "../../general.hpp"
#include "../../song/song.hpp"
#include "../../song/links/manager.hpp"
#include "../../song/links/target.hpp"
#include "../../song/lock/registry.hpp"
#include "../../utils/math.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../utils.hpp"
#include "channels.hpp"

GUIChannelsPanel::GUIChannelsPanel(const bool visible, const bool windowed)
    : GUIPanel("Channels", visible, windowed) {
}

GUIElement GUIChannelsPanel::get_element() const {
    return GUIElement::Channels;
}

void GUIChannelsPanel::draw() {
    ImGui::BeginDisabled(lock_registry.is_locked(Target::CHANNEL, channel_index));
    draw_channel();
    ImGui::EndDisabled();
}

bool GUIChannelsPanel::is_disabled() const {
    return gui.is_playing();
}

bool GUIChannelsPanel::select_item() {
    std::vector<std::pair<ItemType, uint8_t>> link_dependencies = LinkManager::find_dependencies(Target::CHANNEL, channel_index);
    push_tertiary_style();
    draw_add_or_remove({}, link_dependencies);
    prepare_combo(this, channel_names, "##ChannelCombo", channel_index, {}, false, GUI_COMBO_MARGIN_RIGHT);
    lock_item(Target::CHANNEL, channel_index);
    pop_tertiary_style();
    ImGui::Separator();

    return !channels.empty();
}

void GUIChannelsPanel::empty() {
    ImGui::Text("No channel available.");
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
            current_channel.pitch = Song::calculate_real_bpm() / unit * fraction;
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
    if (!is_index_valid()) {
        return;
    }

    if (save && gui.is_playing()) {
        gui.stop();
    }

    if (!save && !focus) {
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
            channel->fraction |= ((current_channel.sync_denominator & 0x0F) << 4U);
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
    Channel *new_channel = Song::add_channel();
    if (new_channel == nullptr) {
        return;
    }

    channel_index = channels.size() - 1;
    perform_action_add(this, {Target::CHANNEL, channel_index, 0});
    update();
}

void GUIChannelsPanel::duplicate() {
    Channel *new_channel = Song::duplicate_channel(channel_index);
    if (new_channel == nullptr) {
        return;
    }

    channel_index = channels.size() - 1;
    update();
}

void GUIChannelsPanel::remove() {
    if (is_index_valid()) {
        perform_action_remove(this, {Target::CHANNEL, channel_index, 0}, channels[channel_index]);
        Song::remove_channel(channel_index);
        channel_index = std::max(0, channel_index - 1);
        update();
    }
}

void GUIChannelsPanel::update() {
    channel_index = clamp_index(channel_index, channels.size());
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
    draw_checkbox(this, "Bypass", current_channel.output_type.bypass, {Target::SPECIAL, channel_index, SPECIAL_CHANNEL_BYPASS});
    draw_checkbox(this, "Hide in pattern view", current_channel.hide, {Target::SPECIAL, channel_index, SPECIAL_CHANNEL_HIDE});
    ImGui::Separator();

    ImGui::Text("Envelope:");
    if (prepare_combo(this, envelope_names, "##EnvelopeCombo", current_channel.envelope_index, {Target::CHANNEL, channel_index, CHANNEL_ENVELOPE_INDEX}, true).right_clicked) {
        gui.set_index(GUIElement::Envelopes, current_channel.envelope_index);
    }

    ImGui::Text("Oscillator:");
    if (prepare_combo(this, oscillator_names, "##OscillatorCombo", current_channel.oscillator_index, {Target::CHANNEL, channel_index, CHANNEL_OSCILLATOR_INDEX}, true).right_clicked) {
        gui.set_index(GUIElement::Oscillators, current_channel.oscillator_index);
    }

    ImGui::Text("Order:");
    if (prepare_combo(this, order_names, "##OrderCombo", current_channel.order_index, {Target::CHANNEL, channel_index, CHANNEL_ORDER_INDEX}, !current_channel.constant_pitch).right_clicked) {
        gui.set_index(GUIElement::Orders, current_channel.order_index);
    }

    ImGui::Separator();
    ImGui::Text("Pitch/transpose:");
    draw_checkbox(this, "Constant pitch", current_channel.constant_pitch, {Target::SPECIAL, channel_index, SPECIAL_CHANNEL_CONSTANT_PITCH});
    ImGui::BeginDisabled(!current_channel.constant_pitch);
    draw_checkbox(this, "Synchronize", current_channel.sync, {Target::SPECIAL, channel_index, SPECIAL_CHANNEL_SYNCHRONIZE});
    ImGui::EndDisabled();

    const LinkKey key = {Target::CHANNEL, channel_index, CHANNEL_PITCH};
    if (current_channel.constant_pitch) {
        if (current_channel.sync) {
            const int previous_numerator = current_channel.sync_numerator;
            const int previous_denominator = current_channel.sync_denominator;
            ImGui::Text("Ratio:");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##Numerator", &current_channel.sync_numerator, 0, 0);
            ImGui::SameLine();
            ImGui::TextUnformatted("/");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(50);
            ImGui::InputInt("##Denominator", &current_channel.sync_denominator, 0, 0);
            current_channel.sync_numerator = clamp(current_channel.sync_numerator, 1, 16);
            current_channel.sync_denominator = clamp(current_channel.sync_denominator, 1, 16);
            perform_action(this, {Target::SPECIAL, channel_index, SPECIAL_CHANNEL_SYNC_NUMERATOR}, current_channel.sync_numerator, previous_numerator);
            perform_action(this, {Target::SPECIAL, channel_index, SPECIAL_CHANNEL_SYNC_DENOMINATOR}, current_channel.sync_denominator, previous_denominator);
        } else {
            draw_float_slider(this, "Pitch (Hz)", current_channel.pitch, key, GUI_MIN_PITCH, GUI_MAX_PITCH, GUIScale::Logarithmic);
        }
    } else {
        draw_float_slider(this, "Transpose", current_channel.transpose, key, GUI_MIN_TRANSPOSE, GUI_MAX_TRANSPOSE);
    }

    if (draw_output(this, current_channel.output_type, {Target::CHANNEL, channel_index, CHANNEL_SPLITTER})) {
        update_channel_name(channel_index, current_channel.output_type.target);
    }
}

void GUIChannelsPanel::set_index(const int index) {
    channel_index = clamp_index(index, channels.size());
}
