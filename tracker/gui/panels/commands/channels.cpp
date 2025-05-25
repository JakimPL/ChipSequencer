#include "../../../general.hpp"
#include "../../names.hpp"
#include "../../utils.hpp"
#include "channels.hpp"

GUICommandsChannelsPanel::GUICommandsChannelsPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUICommandsChannelsPanel::draw() {
    ImGui::Begin("Commands channels");
    ImGui::Columns(1, "commands_channel_columns");

    ImGui::BeginDisabled(gui.is_playing());
    push_tertiary_style();
    draw_add_or_remove();
    prepare_combo(commands_channel_names, "##CommandsChannelCombo", channel_index);
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

bool GUICommandsChannelsPanel::is_index_valid() const {
    return channel_index >= 0 && channel_index < commands_channels.size();
}

void GUICommandsChannelsPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    const CommandsChannel *channel = commands_channels[channel_index];
    current_channel.bypass = channel->flag & FLAG_BYPASS;
    current_channel.order_index = channel->order_index;
}

void GUICommandsChannelsPanel::to() const {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || !is_index_valid() || gui.is_playing()) {
        return;
    }

    CommandsChannel *channel = commands_channels[channel_index];
    channel->flag = current_channel.bypass ? FLAG_BYPASS : 0;
    channel->order_index = current_channel.order_index;
}

void GUICommandsChannelsPanel::add() {
    CommandsChannel *new_channel = song.add_commands_channel();
    if (new_channel == nullptr) {
        return;
    }

    channel_index = commands_channels.size() - 1;
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
        song.remove_commands_channel(channel_index);
        channel_index = std::max(0, channel_index - 1);
        update();
    }
}

void GUICommandsChannelsPanel::update() {
    update_items(commands_channel_names, commands_channels.size(), "Command channel ", channel_index);
    gui.update(GUIElement::Patterns);
}

void GUICommandsChannelsPanel::draw_channel() {
    if (commands_channels.empty()) {
        ImGui::Text("No channels available.");
        return;
    }

    ImGui::Checkbox("Bypass", &current_channel.bypass);
    ImGui::Separator();

    ImGui::Text("Order:");
    if (prepare_combo(order_names, "##OrderCombo", current_channel.order_index, true).right_clicked) {
        gui.set_index(GUIElement::Orders, current_channel.order_index);
    }
}

void GUICommandsChannelsPanel::check_keyboard_input() {
}

void GUICommandsChannelsPanel::set_index(const int index) {
    channel_index = clamp_index(index, commands_channels.size());
}
