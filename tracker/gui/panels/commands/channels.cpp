#include "../../../general.hpp"
#include "../../names.hpp"
#include "../../utils.hpp"
#include "channels.hpp"

GUICommandsChannelsPanel::GUICommandsChannelsPanel(
    const bool visible,
    const bool windowed
)
    : GUIPanel("Commands channels", visible, windowed) {
    initialize();
}

GUIElement GUICommandsChannelsPanel::get_element() const {
    return GUIElement::CommandsChannels;
}

void GUICommandsChannelsPanel::draw() {
    ImGui::BeginDisabled(lock_registry.is_locked(Target::COMMANDS_CHANNEL, channel_index));
    draw_channel();
    ImGui::EndDisabled();
}

bool GUICommandsChannelsPanel::is_disabled() const {
    return gui.is_playing();
}

bool GUICommandsChannelsPanel::select_item() {
    push_tertiary_style();
    draw_add_or_remove();
    prepare_combo(this, commands_channel_names, "##CommandsChannelCombo", channel_index, {}, false, GUI_COMBO_MARGIN_RIGHT);
    lock_item(Target::COMMANDS_CHANNEL, channel_index);
    pop_tertiary_style();
    ImGui::Separator();

    return !commands_channels.empty();
}

void GUICommandsChannelsPanel::empty() {
    ImGui::Text("No command channel available.");
}

bool GUICommandsChannelsPanel::is_index_valid() const {
    return channel_index >= 0 && channel_index < commands_channels.size();
}

void GUICommandsChannelsPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    const CommandsChannel *channel = commands_channels[channel_index];
    current_channel.order_index = channel->order_index;
    current_channel.bypass = channel->flag & FLAG_BYPASS;
    current_channel.hide = channel->flag & FLAG_HIDDEN;
}

void GUICommandsChannelsPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    if (save && gui.is_playing()) {
        gui.stop();
    }

    if (!save && !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    CommandsChannel *channel = commands_channels[channel_index];
    channel->order_index = current_channel.order_index;
    channel->flag = current_channel.bypass ? FLAG_BYPASS : 0;
    channel->flag |= current_channel.hide ? FLAG_HIDDEN : 0;
}

void GUICommandsChannelsPanel::add() {
    CommandsChannel *new_channel = song.add_commands_channel();
    if (new_channel == nullptr) {
        return;
    }

    channel_index = commands_channels.size() - 1;
    perform_action_add(this, {Target::COMMANDS_CHANNEL, channel_index, 0});
    update();
}

void GUICommandsChannelsPanel::duplicate() {
    CommandsChannel *new_channel = song.duplicate_commands_channel(channel_index);
    if (new_channel == nullptr) {
        return;
    }

    channel_index = commands_channels.size() - 1;
    update();
}

void GUICommandsChannelsPanel::remove() {
    if (is_index_valid()) {
        perform_action_remove(this, {Target::COMMANDS_CHANNEL, channel_index, 0}, commands_channels[channel_index]);
        song.remove_commands_channel(channel_index);
        channel_index = std::max(0, channel_index - 1);
        update();
    }
}

void GUICommandsChannelsPanel::update() {
    channel_index = clamp_index(channel_index, commands_channels.size());
    update_items(commands_channel_names, commands_channels.size(), "Command channel ", channel_index);
    gui.update(GUIElement::Patterns);
}

void GUICommandsChannelsPanel::draw_channel() {
    draw_checkbox(this, "Bypass", current_channel.bypass, {Target::SPECIAL, channel_index, SPECIAL_COMMAND_CHANNEL_BYPASS});
    draw_checkbox(this, "Hide in pattern view", current_channel.hide, {Target::SPECIAL, channel_index, SPECIAL_COMMAND_CHANNEL_HIDE});
    ImGui::Separator();

    ImGui::Text("Order:");
    if (prepare_combo(this, order_names, "##OrderCombo", current_channel.order_index, {Target::COMMANDS_CHANNEL, channel_index, COMMANDS_CHANNEL_ORDER_INDEX}, true).right_clicked) {
        gui.set_index(GUIElement::Orders, current_channel.order_index);
    }
}

void GUICommandsChannelsPanel::set_index(const int index) {
    channel_index = clamp_index(index, commands_channels.size());
}
