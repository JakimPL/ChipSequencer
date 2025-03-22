#ifndef GUI_PANELS_CHANNELS_HPP
#define GUI_PANELS_CHANNELS_HPP

#include <algorithm>
#include <cmath>
#include <string>
#include "../init.hpp"
#include "../names.hpp"
#include "utils.hpp"

class GUIChannelsPanel {
  private:
    struct CurrentChannel {
        int envelope_index;
        bool constant_pitch;
        int order_index;
        int oscillator_index;
        float pitch;
        void *output;
        bool additive;
        int type;
        int shift;
    } current_channel;

    int channel_index = 0;

    void from_channel() {
        Channel *channel = channels[channel_index];
        current_channel.envelope_index = channel->envelope_index;
        current_channel.constant_pitch = channel->order_index == 0xFF;
        current_channel.order_index = std::max(0, static_cast<int>(channel->order_index));
        current_channel.oscillator_index = channel->oscillator_index;
        current_channel.additive = !(channel->output_flag & CHANNEL_MASK_ADDITIVE);
        current_channel.type = (channel->output_flag & CHANNEL_MASK_VARIABLE_TYPE) >> 4;
        current_channel.shift = current_channel.type == 0 ? 0 : channel->output_flag & CHANNEL_MASK_SHIFT;
        current_channel.output = channel->output;

        if (current_channel.constant_pitch) {
            current_channel.pitch = static_cast<float>(channel->pitch) / 0x10000;
        } else {
            current_channel.pitch = 12 * log2(static_cast<float>(channel->pitch) / 0x02000000);
        }
    }

    void to_channel() {
        Channel *channel = channels[channel_index];
        channel->envelope_index = current_channel.envelope_index;
        channel->oscillator_index = current_channel.oscillator_index;

        channel->output_flag = current_channel.additive ? 0 : CHANNEL_MASK_ADDITIVE;
        channel->output_flag |= (current_channel.type << 4) | current_channel.shift;
        channel->output = current_channel.output;

        channel->order_index = current_channel.constant_pitch ? CONSTANT_PITCH : current_channel.order_index;
        if (current_channel.constant_pitch) {
            channel->pitch = static_cast<uint32_t>(std::round(current_channel.pitch * 0x10000));
        } else {
            channel->pitch = static_cast<uint32_t>(std::round(0x02000000 * pow(2, current_channel.pitch / 12)));
        }
    }

    void update_channels() {
        update_items(channel_names, channels.size(), "Channel ", channel_index);
        update_items(envelope_names, envelopes.size(), "Envelope ", current_channel.envelope_index);
        update_items(order_names, orders.size(), "Order ", current_channel.order_index);
        update_items(oscillator_names, oscillators.size(), "Oscillator ", current_channel.oscillator_index);
    }

    void draw_channel() {
        if (channels.empty()) {
            ImGui::Text("No channels available.");
            return;
        }

        ImGui::Separator();
        ImGui::Text("Channel:");
        prepare_combo(envelope_names, "##EnvelopeCombo", current_channel.envelope_index);
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
            draw_float_slider("Transpose", current_channel.pitch, -48.0f, 48.0f);
        }

        ImGui::Separator();
        ImGui::Text("Output:");
        ImGui::Checkbox("Additive", &current_channel.additive);
        prepare_combo(variable_types, "##TypeCombo", current_channel.type);
        ImGui::BeginDisabled(current_channel.type == 0);
        draw_int_slider("Shift", current_channel.shift, 0, 15);
        ImGui::EndDisabled();
    }

  public:
    GUIChannelsPanel() {
        update_channels();
    }

    void draw() {
        ImGui::Begin("Channel Editor");
        ImGui::Columns(1, "channel_columns");

        prepare_combo(channel_names, "##ChannelCombo", channel_index);
        from_channel();
        draw_channel();
        to_channel();

        ImGui::Columns(1);
        ImGui::End();
    }
};

#endif // GUI_PANELS_CHANNELS_HPP