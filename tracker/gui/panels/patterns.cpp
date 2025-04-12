#include "../../general.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "patterns.hpp"

#include <iostream>

GUIPatternsPanel::GUIPatternsPanel() {
    initialize();
    from();
    update();
}

void GUIPatternsPanel::initialize() {
}

void GUIPatternsPanel::update() {
}

void GUIPatternsPanel::draw() {
    ImGui::Begin("Pattern Editor");
    ImGui::Separator();

    from();
    draw_channels();
    check_keyboard_input();
    to();

    ImGui::End();
}

void GUIPatternsPanel::draw_pages() {
    const int previous_page = page;
    const int pages = std::ceil(static_cast<float>(current_pattern.total_rows) / gui.get_page_size());
    draw_int_slider("Page", page, {}, 0, pages - 1);
    if (page != previous_page) {
        deselect_all_rows();
    }
}

void GUIPatternsPanel::draw_channels() {
    size_t columns = current_pattern.patterns.size();
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

    if (ImGui::BeginTable("ChannelsTable", num_channels, ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter)) {
        for (const auto &[index, pattern] : current_pattern.patterns) {
            ImGui::TableNextColumn();
            draw_channel(index);
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

    for (auto &pattern : current_pattern.patterns[channel_index]) {
        const int playing_row = current_pattern.playing_rows[channel_index];
        auto [new_index, select] = draw_pattern(pattern, false, index, playing_row, start, end);
        if (select) {
            current_channel = channel_index;
            current_row = pattern.current_row + index;
        }
        index = new_index;
    }

    ImGui::PopID();
}

void GUIPatternsPanel::from() {
    current_pattern.total_rows = 0;
    current_pattern.patterns.clear();
    current_pattern.playing_rows.clear();
    const bool is_playing = gui.is_playing() && ticks_per_beat > 0;
    for (size_t channel_index = 0; channel_index < channels.size(); ++channel_index) {
        const Channel *channel = channels[channel_index];
        const uint8_t order_index = channel->order_index;
        const bool constant_pitch = channel->order_index == CONSTANT_PITCH;
        if (constant_pitch || order_index >= orders.size()) {
            continue;
        }

        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences.begin(), order->sequences.begin() + order->order_length);

        uint16_t row = 0;
        uint8_t playing_sequence = current_sequence[channel_index];
        current_pattern.playing_rows[channel_index] = -1;
        for (size_t j = 0; j < order->order_length; ++j) {
            const uint8_t sequence_index = order_sequences[j];
            if (sequence_index >= sequences.size()) {
                break;
            }

            Pattern pattern = Pattern(sequence_index);
            pattern.current_row = channel_index == current_channel ? current_row - row : -1;
            current_pattern.patterns[channel_index].push_back(pattern);
            if (is_playing && playing_sequence == j) {
                const int playing_row = pattern.calculate_playing_row(channel_index);
                current_pattern.playing_rows[channel_index] = row + playing_row;
            }

            row += pattern.steps;
        }

        current_pattern.total_rows = std::max(current_pattern.total_rows, row);
    }
}

void GUIPatternsPanel::to() const {
    Pattern *pattern = find_pattern_by_current_row().first;
    if (pattern != nullptr) {
        size_t sequence_index = pattern->sequence_index;
        Sequence *sequence = sequences[sequence_index];
        const std::vector<Note> note_vector = pattern->to_note_vector();
        sequence->from_note_vector(note_vector);
    }
}

void GUIPatternsPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    auto [pattern, index] = find_pattern_by_current_row();
    if (pattern != nullptr) {
        const uint16_t start = page * gui.get_page_size() - index;
        const uint16_t end = start + gui.get_page_size() - index;
        pattern->handle_input(start, end);
        current_row = pattern->current_row + index;
    }
}

std::pair<Pattern *, uint16_t> GUIPatternsPanel::find_pattern_by_current_row() const {
    if (current_channel >= current_pattern.patterns.size()) {
        return {nullptr, 0};
    }

    const auto &patterns = current_pattern.patterns.at(current_channel);
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

void GUIPatternsPanel::deselect_all_rows() {
    for (auto &pattern : current_pattern.patterns) {
        for (auto &p : pattern.second) {
            p.current_row = -1;
        }
    }
}
