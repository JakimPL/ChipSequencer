#include "../../general.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "patterns.hpp"

GUIPatternsPanel::GUIPatternsPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUIPatternsPanel::update() {
}

void GUIPatternsPanel::draw() {
    ImGui::Begin("Patterns");
    ImGui::Separator();

    from();
    draw_channels();
    check_keyboard_input();
    to();

    ImGui::End();
}

void GUIPatternsPanel::draw_pages() {
    const int previous_page = page;
    const int pages = get_pages();
    draw_int_slider("Page", page, {}, 0, pages - 1);
    if (page != previous_page) {
        deselect_all_rows();
    }
}

void GUIPatternsPanel::draw_channels() {
    size_t columns = current_patterns.patterns.size() + current_patterns.commands_patterns.size();
    if (columns == 0) {
        ImGui::Text("No channels available.");
        return;
    }

    draw_pages();

    const float available = ImGui::GetContentRegionAvail().x;
    const float total_min = columns * GUI_MINIMAL_CHANNEL_COLUMN_WIDTH;
    const float child_width = (available >= total_min) ? available : total_min;
    const float column_width = (available >= total_min) ? (available / columns) : GUI_MINIMAL_CHANNEL_COLUMN_WIDTH;

    ImGui::SetNextWindowContentSize(ImVec2(std::max(available, total_min), 0));
    ImGui::BeginChild("##PatternChannels", ImVec2(child_width, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (ImGui::BeginTable("ChannelsTable", columns, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter)) {
        for (const auto &[index, pattern] : current_patterns.patterns) {
            ImGui::TableNextColumn();
            draw_channel(index);
        }
        for (const auto &[index, pattern] : current_patterns.commands_patterns) {
            ImGui::TableNextColumn();
            draw_commands_channel(index);
        }
        ImGui::EndTable();
    }

    ImGui::EndChild();
}

void GUIPatternsPanel::draw_channel(size_t channel_index) {
    ImGui::PushID(channel_index);
    ImGui::Text("%s", channel_names[channel_index].c_str());
    size_t index = 0;

    const uint16_t start = page * gui.get_page_size();
    const uint16_t end = start + gui.get_page_size();

    for (auto &pattern : current_patterns.patterns[channel_index]) {
        const int playing_row = current_patterns.playing_rows[{false, channel_index}];
        auto [new_index, select] = draw_pattern(pattern, false, index, playing_row, start, end);
        if (select) {
            current_channel = {false, channel_index};
            current_row = pattern.current_row + index;
        }
        index = new_index;
    }

    ImGui::PopID();
}

void GUIPatternsPanel::draw_commands_channel(size_t channel_index) {
    ImGui::PushID(channel_index);
    ImGui::Text("%s", commands_channel_names[channel_index].c_str());
    size_t index = 0;

    const uint16_t start = page * gui.get_page_size();
    const uint16_t end = start + gui.get_page_size();

    for (auto &pattern : current_patterns.commands_patterns[channel_index]) {
        const int playing_row = current_patterns.playing_rows[{true, channel_index}];
        auto [new_index, select] = draw_commands_pattern(pattern, false, index, playing_row, start, end);
        if (select) {
            current_channel = {true, channel_index};
            current_row = pattern.current_row + index;
        }
        index = new_index;
    }

    ImGui::PopID();
}

void GUIPatternsPanel::from() {
    current_patterns.total_rows = 0;
    current_patterns.playing_rows.clear();
    from_sequences();
    from_commands_sequences();
}

void GUIPatternsPanel::from_sequences() {
    current_patterns.patterns.clear();
    const bool playing = is_playing();
    for (size_t channel_index = 0; channel_index < channels.size(); ++channel_index) {
        const Channel *channel = channels[channel_index];
        const uint8_t order_index = channel->order_index;
        if (order_index >= orders.size()) {
            continue;
        }

        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences.begin(), order->sequences.begin() + order->order_length);

        uint16_t row = 0;
        uint8_t playing_sequence = current_sequence[channel_index];
        current_patterns.playing_rows[{false, channel_index}] = -1;
        for (size_t j = 0; j < order->order_length; ++j) {
            const uint8_t sequence_index = order_sequences[j];
            if (sequence_index >= sequences.size()) {
                break;
            }

            current_patterns.patterns[channel_index].emplace_back(sequence_index);
            Pattern &pattern = current_patterns.patterns[channel_index].back();
            pattern.current_row = !current_channel.command && channel_index == current_channel.index ? current_row - row : -1;
            if (playing && playing_sequence == j) {
                const int playing_row = pattern.calculate_playing_row(channel_index);
                current_patterns.playing_rows[{false, channel_index}] = row + playing_row;
            }

            row += pattern.steps;
        }

        current_patterns.total_rows = std::max(current_patterns.total_rows, row);
    }
}

void GUIPatternsPanel::from_commands_sequences() {
    const bool playing = is_playing();
    for (size_t channel_index = 0; channel_index < commands_channels.size(); ++channel_index) {
        const CommandsChannel *channel = commands_channels[channel_index];
        const uint8_t order_index = channel->order_index;
        if (order_index >= orders.size()) {
            continue;
        }

        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences.begin(), order->sequences.begin() + order->order_length);

        uint16_t row = 0;
        uint8_t playing_sequence = current_commands_sequence[channel_index];
        current_patterns.playing_rows[{true, channel_index}] = -1;
        for (size_t j = 0; j < order->order_length; ++j) {
            const uint8_t sequence_index = order_sequences[j];
            if (sequence_index >= commands_sequences.size()) {
                break;
            }

            if (j >= current_patterns.commands_patterns[channel_index].size()) {
                current_patterns.commands_patterns[channel_index].emplace_back(sequence_index);
            } else {
                current_patterns.commands_patterns[channel_index][j].from_sequence(sequence_index);
            }

            CommandsPattern &pattern = current_patterns.commands_patterns[channel_index][j];
            pattern.current_row = current_channel.command && channel_index == current_channel.index ? current_row - row : -1;
            if (playing && playing_sequence == j) {
                const int playing_row = pattern.calculate_playing_row(channel_index);
                current_patterns.playing_rows[{true, channel_index}] = row + playing_row;
            }

            row += pattern.steps;
        }

        current_patterns.total_rows = std::max(current_patterns.total_rows, row);
        if (current_patterns.commands_patterns[channel_index].size() > order->order_length) {
            current_patterns.commands_patterns[channel_index].resize(order->order_length);
        }
    }

    for (auto it = current_patterns.commands_patterns.begin(); it != current_patterns.commands_patterns.end();) {
        if (it->first < 0 || it->first >= commands_channels.size()) {
            it = current_patterns.commands_patterns.erase(it);
        } else {
            ++it;
        }
    }
}

void GUIPatternsPanel::to() const {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    to_sequences();
    to_commands_sequences();
}

void GUIPatternsPanel::to_sequences() const {
    Pattern *pattern = find_pattern_by_current_row().first;
    if (pattern != nullptr) {
        size_t sequence_index = pattern->sequence_index;
        Sequence *sequence = sequences[sequence_index];
        const std::vector<Note> note_vector = pattern->to_note_vector();
        sequence->from_note_vector(note_vector);
    }
}

void GUIPatternsPanel::to_commands_sequences() const {
    CommandsPattern *pattern = find_commands_pattern_by_current_row().first;
    if (pattern != nullptr) {
        size_t sequence_index = pattern->sequence_index;
        CommandsSequence *sequence = commands_sequences[sequence_index];
        const std::vector<Command> command_vector = pattern->to_command_vector();
        sequence->from_command_vector(command_vector);
    }
}

void GUIPatternsPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_PageDown)) {
        page = std::min(page + 1, get_pages() - 1);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_PageUp)) {
        page = std::max(page - 1, 0);
    }

    auto [pattern, index] = find_pattern_by_current_row();
    auto [commands_pattern, commands_index] = find_commands_pattern_by_current_row();
    if (pattern != nullptr) {
        handle_pattern_input(pattern, index);
    } else if (commands_pattern != nullptr) {
        handle_commands_pattern_input(commands_pattern, commands_index);
    }
}

void GUIPatternsPanel::handle_pattern_input(Pattern *pattern, const uint16_t index) {
    if (!current_patterns.patterns.empty()) {
        auto it = current_patterns.patterns.find(current_channel.index);
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
            if (it == current_patterns.patterns.begin()) {
                if (current_patterns.commands_patterns.empty()) {
                    current_channel.index = current_patterns.patterns.rbegin()->first;
                } else {
                    current_channel.command = true;
                    current_channel.index = current_patterns.commands_patterns.rbegin()->first;
                    CommandsPattern *commands_pattern = find_commands_pattern_by_current_row().first;
                    commands_pattern->selection = CommandSelection::Value;
                }
            } else {
                --it;
                current_channel.index = it->first;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) {
            if (std::next(it) == current_patterns.patterns.end()) {
                if (current_patterns.commands_patterns.empty()) {
                    current_channel.index = current_patterns.patterns.begin()->first;
                } else {
                    current_channel.command = true;
                    current_channel.index = current_patterns.commands_patterns.begin()->first;
                    CommandsPattern *commands_pattern = find_commands_pattern_by_current_row().first;
                    commands_pattern->selection = CommandSelection::Command;
                }
            } else {
                ++it;
                current_channel.index = it->first;
            }
        }
    }

    const int start = page * gui.get_page_size() - index;
    const int end = start + gui.get_page_size();
    pattern->handle_input(start, end);
    current_row = pattern->current_row + index;
}

void GUIPatternsPanel::handle_commands_pattern_input(CommandsPattern *pattern, const uint16_t index) {
    if (!current_patterns.commands_patterns.empty()) {
        auto it = current_patterns.commands_patterns.find(current_channel.index);
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow) && pattern->selection == CommandSelection::Command) {
            if (it == current_patterns.commands_patterns.begin()) {
                if (current_patterns.patterns.empty()) {
                    current_channel.index = current_patterns.commands_patterns.rbegin()->first;
                } else {
                    current_channel.command = false;
                    current_channel.index = current_patterns.patterns.rbegin()->first;
                }
            } else {
                --it;
                current_channel.index = it->first;
                CommandsPattern *commands_pattern = find_commands_pattern_by_current_row().first;
                commands_pattern->selection = CommandSelection::Value;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_RightArrow) && pattern->selection == CommandSelection::Value) {
            if (std::next(it) == current_patterns.commands_patterns.end()) {
                if (current_patterns.patterns.empty()) {
                    current_channel.index = current_patterns.commands_patterns.begin()->first;
                } else {
                    current_channel.command = false;
                    current_channel.index = current_patterns.patterns.begin()->first;
                }
            } else {
                ++it;
                current_channel.index = it->first;
                CommandsPattern *commands_pattern = find_commands_pattern_by_current_row().first;
                commands_pattern->selection = CommandSelection::Command;
            }
        }
    }

    const int start = page * gui.get_page_size() - index;
    const int end = start + gui.get_page_size();
    pattern->handle_input(start, end);
    current_row = pattern->current_row + index;
}

std::pair<Pattern *, uint16_t> GUIPatternsPanel::find_pattern_by_current_row() const {
    if (current_channel.command) {
        return {nullptr, 0};
    }

    const auto it = current_patterns.patterns.find(current_channel.index);
    if (it == current_patterns.patterns.end()) {
        return {nullptr, 0};
    }

    const auto &patterns = current_patterns.patterns.at(current_channel.index);
    uint16_t rows = 0;
    size_t pattern_id;
    for (pattern_id = 0; pattern_id < patterns.size(); ++pattern_id) {
        rows += patterns[pattern_id].steps;
        if (rows > current_row) {
            rows -= patterns[pattern_id].steps;
            break;
        }
    }

    if (pattern_id >= patterns.size()) {
        return {nullptr, 0};
    }

    Pattern *pattern = const_cast<Pattern *>(&patterns.at(pattern_id));
    return {pattern, rows};
}

std::pair<CommandsPattern *, uint16_t> GUIPatternsPanel::find_commands_pattern_by_current_row() const {
    if (!current_channel.command) {
        return {nullptr, 0};
    }

    const auto it = current_patterns.commands_patterns.find(current_channel.index);
    if (it == current_patterns.commands_patterns.end()) {
        return {nullptr, 0};
    }

    const auto &patterns = current_patterns.commands_patterns.at(current_channel.index);
    uint16_t rows = 0;
    size_t pattern_id;
    for (pattern_id = 0; pattern_id < patterns.size(); ++pattern_id) {
        rows += patterns[pattern_id].steps;
        if (rows > current_row) {
            rows -= patterns[pattern_id].steps;
            break;
        }
    }

    if (pattern_id >= patterns.size()) {
        return {nullptr, 0};
    }

    CommandsPattern *pattern = const_cast<CommandsPattern *>(&patterns.at(pattern_id));
    return {pattern, rows};
}

int GUIPatternsPanel::get_pages() const {
    return std::ceil(static_cast<float>(current_patterns.total_rows) / gui.get_page_size());
}

void GUIPatternsPanel::deselect_all_rows() {
    for (auto &pattern : current_patterns.patterns) {
        for (auto &p : pattern.second) {
            p.current_row = -1;
        }
    }
    for (auto &pattern : current_patterns.commands_patterns) {
        for (auto &p : pattern.second) {
            p.current_row = -1;
            p.selection = CommandSelection::None;
        }
    }
}

void GUIPatternsPanel::set_index(const int index) {
    current_index = static_cast<size_t>(index);
}

bool GUIPatternsPanel::is_playing() const {
    return gui.is_playing() && ticks_per_beat > 0;
}
