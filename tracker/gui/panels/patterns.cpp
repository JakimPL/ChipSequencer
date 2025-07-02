#include <optional>

#include "../../general.hpp"
#include "../../song/song.hpp"
#include "../../song/lock/registry.hpp"
#include "../../utils/math.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../utils.hpp"
#include "../clipboard/clipboard.hpp"
#include "../clipboard/items/notes.hpp"
#include "patterns.hpp"

GUIPatternsPanel::GUIPatternsPanel(const bool visible, const bool windowed)
    : GUIPanel("Patterns", visible, windowed) {
}

GUIElement GUIPatternsPanel::get_element() const {
    return GUIElement::Patterns;
}

void GUIPatternsPanel::draw() {
    draw_pages();
    draw_channels();
}

void GUIPatternsPanel::draw_pages() {
    const int previous_page = page;
    const int pages = std::max(1, get_pages());

    if (gui.follow_playback && gui.is_playing()) {
        const int total_rows = current_patterns.total_rows;
        page = total_rows > 0 ? mod(global_row, total_rows) / gui.get_page_size() : 0;
    }

    if (draw_int_slider(this, "Page", page, {}, 0, pages - 1)) {
        gui.follow_playback = false;
    }

    if (page != previous_page) {
        deselect_all_rows();
    }

    page = clamp(page, 0, pages - 1);
}

void GUIPatternsPanel::draw_channels() {
    size_t columns = current_patterns.patterns.size() + current_patterns.commands_patterns.size();
    if (columns == 0) {
        ImGui::Text("No channels available.");
        return;
    }

    const float available = ImGui::GetContentRegionAvail().x;
    const float total_min = columns * GUI_MINIMAL_CHANNEL_COLUMN_WIDTH;
    const float content_size = std::max(available, total_min);

    ImGui::SetNextWindowContentSize(ImVec2(content_size, 0));
    ImGui::BeginChild("##PatternChannels", ImVec2(available, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

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

    const uint16_t start = page * gui.get_page_size();
    const uint16_t end = start + gui.get_page_size();

    PatternSelection empty_selection = {selection.command};
    PatternSelection &pattern_selection = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ? selection : empty_selection;

    int row = 0;
    for (auto &pattern : current_patterns.patterns[channel_index]) {
        const int playing_row = current_patterns.playing_rows[{false, channel_index}];
        const bool locked = lock_registry.is_locked(Target::SEQUENCE, pattern.sequence_index);
        auto [new_row, select] = draw_pattern(
            pattern, pattern_selection, secondary_sequence_rows, true, channel_index, false, row, playing_row, start, end, locked
        );
        if (select) {
            current_channel = {false, channel_index};
            current_row = pattern.current_row + row;
        }
        row = new_row;
    }

    ImGui::PopID();
}

void GUIPatternsPanel::draw_commands_channel(size_t channel_index) {
    ImGui::PushID(channel_index);
    ImGui::Text("%s", commands_channel_names[channel_index].c_str());

    const uint16_t start = page * gui.get_page_size();
    const uint16_t end = start + gui.get_page_size();

    PatternSelection empty_selection = {selection.command};
    PatternSelection &commands_selection = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ? selection : empty_selection;

    int row = 0;
    for (auto &pattern : current_patterns.commands_patterns[channel_index]) {
        const int playing_row = current_patterns.playing_rows[{true, channel_index}];
        const bool locked = lock_registry.is_locked(Target::COMMANDS_SEQUENCE, pattern.sequence_index);
        auto [new_row, select] = draw_commands_pattern(
            pattern, commands_selection, secondary_sequence_rows, true, channel_index, false, row, playing_row, start, end, locked
        );
        if (select) {
            current_channel = {true, channel_index};
            current_row = pattern.current_row + row;
        }
        row = new_row;
    }

    ImGui::PopID();
}

void GUIPatternsPanel::from() {
    clear();
    from_sequences();
    from_commands_sequences();
    add_repeated_patterns();
    add_repeated_commands_patterns();
}

void GUIPatternsPanel::clear() {
    current_patterns.total_rows = 0;
    current_patterns.playing_rows.clear();
    pattern_rows.clear();
    pattern_rows_by_sequence_row.clear();
    selection_starts.clear();
    current_patterns.patterns.clear();
    current_patterns.patterns_max_rows.clear();
    current_patterns.commands_patterns_max_rows.clear();
}

void GUIPatternsPanel::from_sequences() {
    for (size_t channel_index = 0; channel_index < channels.size(); ++channel_index) {
        const Channel *channel = channels[channel_index];
        const uint8_t order_index = channel->order_index;
        if (order_index >= orders.size() || channel->flag & FLAG_HIDDEN) {
            continue;
        }

        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences.begin(), order->sequences.begin() + order->order_length);

        uint16_t row = 0;
        current_patterns.playing_rows[{false, channel_index}] = -1;
        for (size_t j = 0; j < order->order_length; ++j) {
            process_sequence(channel_index, j, order_sequences[j], row);
        }

        current_patterns.patterns_max_rows[channel_index] = row;
        current_patterns.total_rows = std::max(current_patterns.total_rows, row);
    }
}

void GUIPatternsPanel::process_sequence(
    const size_t channel_index,
    const size_t j,
    const uint8_t sequence_index,
    uint16_t &row
) {
    if (sequence_index >= sequences.size()) {
        return;
    }

    const bool playing = is_playing();
    const uint8_t playing_sequence = current_sequence[channel_index];

    current_patterns.patterns[channel_index].emplace_back(sequence_index);
    const size_t pattern_id = current_patterns.patterns[channel_index].size() - 1;
    Pattern &pattern = current_patterns.patterns[channel_index][pattern_id];
    pattern.id = j;
    pattern.starting_row = row;
    pattern.current_row = !current_channel.command && channel_index == current_channel.index ? current_row - row : -1;

    if (playing) {
        if (gui.repeat_patterns) {
            current_patterns.playing_rows[{false, channel_index}] = global_row;
        } else if (playing_sequence == j) {
            const int playing_row = pattern.calculate_playing_row(channel_index);
            current_patterns.playing_rows[{false, channel_index}] = row + playing_row;
        }
    }

    mark_selected_rows(false, channel_index, pattern_id, row);
    row += pattern.steps;
}

void GUIPatternsPanel::from_commands_sequences() {
    for (size_t channel_index = 0; channel_index < commands_channels.size(); ++channel_index) {
        const CommandsChannel *channel = commands_channels[channel_index];
        const uint8_t order_index = channel->order_index;
        if (channel->flag & FLAG_HIDDEN) {
            current_patterns.commands_patterns.erase(channel_index);
            continue;
        }

        if (order_index >= orders.size()) {
            continue;
        }

        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences.begin(), order->sequences.begin() + order->order_length);

        uint16_t row = 0;
        current_patterns.playing_rows[{true, channel_index}] = -1;
        for (size_t j = 0; j < order->order_length; ++j) {
            process_commands_sequence(channel_index, j, order_sequences[j], row);
        }

        current_patterns.commands_patterns_max_rows[channel_index] = row;
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

void GUIPatternsPanel::process_commands_sequence(
    const size_t channel_index,
    const size_t j,
    const uint8_t sequence_index,
    uint16_t &row
) {
    if (sequence_index >= commands_sequences.size()) {
        return;
    }

    if (j >= current_patterns.commands_patterns[channel_index].size()) {
        current_patterns.commands_patterns[channel_index].emplace_back(sequence_index);
    } else {
        current_patterns.commands_patterns[channel_index][j].from_sequence(sequence_index);
    }

    const bool playing = is_playing();
    const uint8_t playing_sequence = current_commands_sequence[channel_index];

    CommandsPattern &pattern = current_patterns.commands_patterns[channel_index][j];
    pattern.id = j;
    pattern.starting_row = row;
    pattern.current_row = current_channel.command && channel_index == current_channel.index ? current_row - row : -1;

    if (playing) {
        if (gui.repeat_patterns) {
            current_patterns.playing_rows[{true, channel_index}] = global_row;
        } else if (playing_sequence == j) {
            const int playing_row = pattern.calculate_playing_row(channel_index);
            current_patterns.playing_rows[{true, channel_index}] = row + playing_row;
        }
    }

    mark_selected_rows(true, channel_index, j, row);
    row += pattern.steps;
}

void GUIPatternsPanel::add_repeated_patterns() {
    if (!gui.repeat_patterns) {
        return;
    }

    for (const auto &[channel_index, patterns] : current_patterns.patterns) {
        uint16_t row = current_patterns.patterns_max_rows[channel_index];
        if (patterns.empty() || row >= current_patterns.total_rows) {
            continue;
        }

        Channel *channel = channels[channel_index];
        const uint8_t order_index = channel->order_index;
        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences.begin(), order->sequences.begin() + order->order_length);

        size_t k = current_patterns.patterns.size();
        while (row < current_patterns.total_rows) {
            bool limit_exceeded = false;
            for (size_t j = 0; j < order->order_length; ++j) {
                process_sequence(channel_index, k, order_sequences[j], row);
                if (row >= current_patterns.total_rows) {
                    limit_exceeded = true;
                    break;
                }

                k++;
            }

            if (limit_exceeded) {
                break;
            }
        }
    }
}

void GUIPatternsPanel::add_repeated_commands_patterns() {
    if (!gui.repeat_patterns) {
        return;
    }

    for (const auto &[channel_index, patterns] : current_patterns.commands_patterns) {
        uint16_t row = current_patterns.commands_patterns_max_rows[channel_index];
        if (patterns.empty() || row >= current_patterns.total_rows) {
            continue;
        }

        CommandsChannel *channel = commands_channels[channel_index];
        const uint8_t order_index = channel->order_index;
        const Order *order = orders[order_index];
        std::vector<uint8_t> order_sequences = std::vector<uint8_t>(order->sequences.begin(), order->sequences.begin() + order->order_length);

        size_t k = current_patterns.commands_patterns.size();
        while (row < current_patterns.total_rows) {
            bool limit_exceeded = false;
            for (size_t j = 0; j < order->order_length; ++j) {
                process_commands_sequence(channel_index, k, order_sequences[j], row);
                if (row >= current_patterns.total_rows) {
                    limit_exceeded = true;
                    break;
                }

                k++;
            }

            if (limit_exceeded) {
                break;
            }
        }
    }
}

void GUIPatternsPanel::to() const {
    if (!save && !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    to_sequences();
    to_commands_sequences();
}

void GUIPatternsPanel::shortcut_actions() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    switch (selection_action) {
    case PatternSelectionAction::TransposeUp: {
        transpose_selected_rows(1);
        break;
    }
    case PatternSelectionAction::TransposeDown: {
        transpose_selected_rows(-1);
        break;
    }
    case PatternSelectionAction::TransposeOctaveUp: {
        transpose_selected_rows(scale_composer.get_edo());
        break;
    }
    case PatternSelectionAction::TransposeOctaveDown: {
        transpose_selected_rows(-scale_composer.get_edo());
        break;
    }
    case PatternSelectionAction::SelectAll: {
        select_all();
        break;
    }
    case PatternSelectionAction::SelectChannel: {
        select_channel();
        break;
    }
    case PatternSelectionAction::DeselectAll: {
        deselect_all();
        break;
    }
    case PatternSelectionAction::Delete: {
        delete_selection();
        break;
    }
    case PatternSelectionAction::Cut: {
        copy_selection();
        delete_selection();
        break;
    };
    case PatternSelectionAction::Copy: {
        copy_selection();
        break;
    }
    case PatternSelectionAction::Paste: {
        paste_selection();
        break;
    }
    case PatternSelectionAction::SetNoteRest: {
        set_selection_note(NOTE_REST);
        break;
    }
    case PatternSelectionAction::SetNoteCut: {
        set_selection_note(NOTE_CUT);
        break;
    }
    case PatternSelectionAction::SetNoteOff: {
        set_selection_note(NOTE_OFF);
        break;
    }
    case PatternSelectionAction::None:
    default: {
        break;
    }
    }
}

void GUIPatternsPanel::select_all() {
    const bool command = current_channel.command;
    const auto [start, end] = gui.get_page_start_end(page);
    const size_t last_channel_index = current_patterns.patterns.empty() ? 0 : current_patterns.patterns.rbegin()->first;
    selection.select(start, end, command, 0, last_channel_index);
}

void GUIPatternsPanel::select_channel() {
    const bool command = current_channel.command;
    const auto [start, end] = gui.get_page_start_end(page);
    selection.select(start, end, command, current_channel.index, current_channel.index);
}

void GUIPatternsPanel::deselect_all() {
    selection.clear();
}

void GUIPatternsPanel::set_selection_note(const uint8_t note) {
    if (selection.is_active()) {
        for (const auto &[sequence_row, pattern_rows] : pattern_rows_by_sequence_row) {
            for (const PatternRow &pattern_row : pattern_rows) {
                const size_t channel_index = pattern_row.channel_index;
                const size_t pattern_id = pattern_row.pattern_id;
                const int row = pattern_row.row;
                Pattern &pattern = current_patterns.patterns[channel_index][pattern_id];
                const uint8_t row_note = (selection_starts.find(sequence_row) != selection_starts.end()) ? note : NOTE_REST;
                pattern.set_note(row, row_note);
            }
        }
    } else {
        auto [pattern, pattern_id, index] = find_pattern_by_current_row();
        if (pattern != nullptr) {
            pattern->set_note(pattern->current_row, note);
        }
    }
}

void GUIPatternsPanel::copy_selection() {
    if (!selection.is_active() || selection.command) {
        return;
    }

    std::optional<size_t> current_channel_index;
    PatternNotes pattern_notes;
    std::vector<uint8_t> notes;
    for (const auto &pattern_row : pattern_rows) {
        if (!current_channel_index.has_value() || current_channel_index.value() != pattern_row.channel_index) {
            if (current_channel_index.has_value()) {
                pattern_notes.push_back(notes);
            }

            notes.clear();
            current_channel_index = pattern_row.channel_index;
        }

        Pattern &pattern = current_patterns.patterns[pattern_row.channel_index][pattern_row.pattern_id];
        const uint8_t note = pattern.get_note(pattern_row.row);
        notes.push_back(note);
    }

    pattern_notes.push_back(notes);
    clipboard.add_item(
        std::make_unique<ClipboardNotes>("Pattern Notes", pattern_notes)
    );
}

void GUIPatternsPanel::paste_selection() {
    ClipboardItem *item = clipboard.get_recent_item(ClipboardCategory::Notes);
    ClipboardNotes *notes = dynamic_cast<ClipboardNotes *>(item);
    if (!notes || current_channel.command) {
        return;
    }

    pattern_rows_by_sequence_row.clear();
    PatternNotes &pattern_notes = notes->pattern_notes;
    std::map<SequenceRow, uint8_t> selection_notes;
    for (const auto &notes : pattern_notes) {
        if (notes.empty()) {
            continue;
        }

        auto [pattern, pattern_id, index] = find_pattern_by_current_row();
        for (size_t i = 0; i < notes.size(); ++i) {
            const uint8_t note = notes[i];
            const int row = current_row + i;
            int j = row - index;
            if (j >= pattern->notes.size()) {
                pattern_id++;
                if (pattern_id >= current_patterns.patterns[current_channel.index].size()) {
                    break;
                }

                const size_t size = pattern->notes.size();
                pattern = &current_patterns.patterns[current_channel.index][pattern_id];
                index += size;
                j -= size;
            }

            const SequenceRow sequence_row = {pattern->sequence_index, j};
            const PatternRow pattern_row = {current_channel.index, pattern_id, j};
            selection_notes[sequence_row] = note;
            pattern_rows.insert(pattern_row);
            pattern_rows_by_sequence_row[sequence_row].insert(pattern_row);
        }

        auto it = current_patterns.patterns.find(current_channel.index);
        if (it != current_patterns.patterns.end()) {
            ++it;
            if (it == current_patterns.patterns.end()) {
                break;
            }
            current_channel.index = it->first;
        } else {
            break;
        }
    }

    PatternSelectionChange<uint8_t> changes;
    prepare_secondary_selection();
    for (const auto &[sequence_row, pattern_rows] : pattern_rows_by_sequence_row) {
        for (const PatternRow &pattern_row : pattern_rows) {
            Pattern &pattern = current_patterns.patterns[pattern_row.channel_index][pattern_row.pattern_id];
            const uint8_t old_note = pattern.get_note(sequence_row.row);
            const uint8_t new_note = selection_notes[sequence_row];
            pattern.set_note(sequence_row.row, new_note);
            changes[pattern_row] = {old_note, new_note};
        }
    }

    perform_notes_action("Paste", changes);
}

void GUIPatternsPanel::delete_selection() {
    PatternSelectionChange<uint8_t> changes;
    PatternSelectionChange<CommandValue> commands_changes;
    for (const PatternRow &pattern_row : secondary_pattern_rows) {
        const size_t channel_index = pattern_row.channel_index;
        const size_t pattern_id = pattern_row.pattern_id;
        const int row = pattern_row.row;
        if (selection.command) {
            CommandsPattern &pattern = current_patterns.commands_patterns[channel_index][pattern_id];
            const CommandValue old_command = pattern.get_command(row);
            pattern.clear_row(row);
            const CommandValue new_command = pattern.get_command(row);
            commands_changes[pattern_row] = {old_command, new_command};
        } else {
            Pattern &pattern = current_patterns.patterns[channel_index][pattern_id];
            const uint8_t old_note = pattern.get_note(pattern_row.row);
            pattern.clear_row(row);
            const uint8_t new_note = pattern.get_note(pattern_row.row);
            changes[pattern_row] = {old_note, new_note};
        }
    }

    if (selection.command) {
        perform_commands_action("Delete", commands_changes);
    } else {
        perform_notes_action("Delete", changes);
    }
}

void GUIPatternsPanel::transpose_selected_rows(const int value) {
    if (value == 0 || !ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    for (const PatternRow &pattern_row : secondary_pattern_rows) {
        const size_t channel_index = pattern_row.channel_index;
        const size_t pattern_id = pattern_row.pattern_id;
        const int row = pattern_row.row;
        Pattern &pattern = current_patterns.patterns[channel_index][pattern_id];
        pattern.transpose(value, row);
    }
}

void GUIPatternsPanel::to_sequences() const {
    std::set<const Pattern *> unique_patterns;
    if (!selection.command && selection_action != PatternSelectionAction::None && !selection.selecting) {
        for (const auto &[channel_index, pattern_id, row] : secondary_pattern_rows) {
            const Pattern &selected_pattern = current_patterns.patterns.at(channel_index).at(pattern_id);
            if (!lock_registry.is_locked(Target::SEQUENCE, selected_pattern.sequence_index)) {
                unique_patterns.insert(&selected_pattern);
            }
        }
    } else {
        const Pattern *pattern = find_pattern_by_current_row().pattern;
        if (pattern != nullptr) {
            unique_patterns.insert(pattern);
        }
    }

    for (const Pattern *pattern : unique_patterns) {
        size_t sequence_index = pattern->sequence_index;
        Sequence *sequence = sequences[sequence_index];
        const std::vector<Note> note_vector = pattern->to_note_vector();
        sequence->from_note_vector(note_vector);
    }
}

void GUIPatternsPanel::to_commands_sequences() const {
    std::set<const CommandsPattern *> unique_patterns;
    if (selection.command && selection_action != PatternSelectionAction::None && !selection.selecting) {
        for (const auto &[channel_index, pattern_id, row] : secondary_pattern_rows) {
            const CommandsPattern &selected_pattern = current_patterns.commands_patterns.at(channel_index).at(pattern_id);
            if (!lock_registry.is_locked(Target::COMMANDS_SEQUENCE, selected_pattern.sequence_index)) {
                unique_patterns.insert(&selected_pattern);
            }
        }
    } else {
        const CommandsPattern *pattern = find_commands_pattern_by_current_row().pattern;
        if (pattern != nullptr) {
            unique_patterns.insert(pattern);
        }
    }

    for (const CommandsPattern *pattern : unique_patterns) {
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
        gui.follow_playback = false;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_PageUp)) {
        page = std::max(page - 1, 0);
        gui.follow_playback = false;
    }

    auto [pattern, pattern_id, index] = find_pattern_by_current_row();
    auto [commands_pattern, commands_pattern_id, commands_index] = find_commands_pattern_by_current_row();
    if (pattern != nullptr) {
        const int old_row = pattern->current_row;
        const uint8_t old_note = pattern->get_note(old_row);
        handle_pattern_input(pattern, index);
        if (old_note != NOTES) {
            const uint8_t new_note = pattern->notes[old_row];
            const PatternRow pattern_row = {current_channel.index, pattern_id, old_row};
            const uint16_t offset = SEQUENCE_NOTES + sizeof(Note) * (current_row - index);
            const LinkKey key = {Target::SEQUENCE, pattern->sequence_index, offset};
            perform_action_note(this, key, pattern_row, old_note, new_note);
        }
    } else if (commands_pattern != nullptr) {
        const CommandValue old_command = commands_pattern->get_command(commands_index);
        handle_commands_pattern_input(commands_pattern, commands_index);
        const CommandValue new_command = commands_pattern->get_command(commands_index);
        const uint16_t offset = COMMANDS_SEQUENCE_DATA + sizeof(Command) * (current_row - commands_index);
        const LinkKey key = {Target::COMMANDS_SEQUENCE, commands_pattern->sequence_index, offset};
        const PatternRow pattern_row = {current_channel.index, commands_pattern_id, commands_index};
        perform_action_command(this, key, pattern_row, old_command, new_command);
    }
}

void GUIPatternsPanel::handle_pattern_input(Pattern *pattern, uint16_t index) {
    if (!current_patterns.patterns.empty()) {
        auto it = current_patterns.patterns.find(current_channel.index);
        if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) {
            if (it == current_patterns.patterns.begin()) {
                if (current_patterns.commands_patterns.empty()) {
                    current_channel.index = current_patterns.patterns.rbegin()->first;
                } else {
                    current_channel.command = true;
                    current_channel.index = current_patterns.commands_patterns.rbegin()->first;
                    CommandsPattern *commands_pattern = find_commands_pattern_by_current_row().pattern;
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
                    CommandsPattern *commands_pattern = find_commands_pattern_by_current_row().pattern;
                    commands_pattern->selection = CommandSelection::Command;
                }
            } else {
                ++it;
                current_channel.index = it->first;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
            if (pattern->current_row == 0 && index > 0) {
                index--;
                current_row--;
                Pattern *previous_pattern = find_pattern_by_current_row().pattern;
                previous_pattern->current_row = previous_pattern->steps - 1;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
            if (
                pattern->current_row == pattern->steps - 1 &&
                index + pattern->steps < current_patterns.patterns_max_rows[current_channel.index]
            ) {
                index++;
                current_row++;
                Pattern *next_pattern = find_pattern_by_current_row().pattern;
                next_pattern->current_row = 0;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Home)) {
            const int start = page * gui.get_page_size();
            index = start;
            current_row = start;
            const auto &[first_pattern, _, new_index] = find_pattern_by_current_row();
            first_pattern->current_row = new_index;
            return;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_End)) {
            const int end_row = std::min(
                (page + 1) * gui.get_page_size() - 1,
                current_patterns.patterns_max_rows[current_channel.index] - 1
            );
            index = end_row;
            current_row = end_row;
            const auto &[last_pattern, _, new_index] = find_pattern_by_current_row();
            last_pattern->current_row = current_row - new_index;
            return;
        }
    }

    const int start = page * gui.get_page_size() - index;
    const int end = start + gui.get_page_size();
    pattern->handle_input(start, end);
    current_row = pattern->current_row + index;
}

void GUIPatternsPanel::handle_commands_pattern_input(CommandsPattern *pattern, uint16_t index) {
    const CommandSelection current_selection = pattern->selection;
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
                CommandsPattern *commands_pattern = find_commands_pattern_by_current_row().pattern;
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
                CommandsPattern *commands_pattern = find_commands_pattern_by_current_row().pattern;
                commands_pattern->selection = CommandSelection::Command;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
            if (pattern->current_row == 0 && index > 0) {
                index--;
                current_row--;
                CommandsPattern *previous_pattern = find_commands_pattern_by_current_row().pattern;
                previous_pattern->current_row = previous_pattern->steps - 1;
                previous_pattern->selection = current_selection;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
            if (
                pattern->current_row == pattern->steps - 1 &&
                index + pattern->steps < current_patterns.commands_patterns_max_rows[current_channel.index]
            ) {
                index++;
                current_row++;
                CommandsPattern *next_pattern = find_commands_pattern_by_current_row().pattern;
                next_pattern->current_row = 0;
                next_pattern->selection = current_selection;
            }
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Home)) {
            const int start = page * gui.get_page_size();
            index = start;
            current_row = start;
            const auto &[first_pattern, _, new_index] = find_commands_pattern_by_current_row();
            first_pattern->current_row = new_index;
            first_pattern->selection = current_selection;
            return;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_End)) {
            const int end_row = std::min(
                (page + 1) * gui.get_page_size() - 1,
                current_patterns.commands_patterns_max_rows[current_channel.index] - 1
            );
            index = end_row;
            current_row = end_row;
            const auto &[last_pattern, _, new_index] = find_commands_pattern_by_current_row();
            last_pattern->current_row = current_row - new_index;
            last_pattern->selection = current_selection;
            return;
        }
    }

    const int start = page * gui.get_page_size() - index;
    const int end = start + gui.get_page_size();
    pattern->handle_input(start, end);
    current_row = pattern->current_row + index;
}

void GUIPatternsPanel::mark_selected_rows(const bool command, const PatternRow &pattern_row) {
    mark_selected_rows(command, pattern_row.channel_index, pattern_row.pattern_id, pattern_row.row);
}

void GUIPatternsPanel::mark_selected_rows(const bool command, const size_t channel_index, const size_t pattern_id, const int row) {
    if (command) {
        mark_selected_commands_pattern_rows(channel_index, pattern_id, row);
    } else {
        mark_selected_pattern_rows(channel_index, pattern_id, row);
    }
}

void GUIPatternsPanel::mark_selected_pattern_rows(const size_t channel_index, const size_t pattern_id, const int row) {
    if (selection.command) {
        return;
    }

    const Pattern &pattern = current_patterns.patterns[channel_index][pattern_id];
    for (int i = 0; i < pattern.notes.size(); ++i) {
        const int j = row + i;
        const PatternRow pattern_row = {channel_index, pattern_id, i};
        if (selection.is_row_selected(channel_index, j) ||
            (!selection.is_active() && pattern.current_row == i)) {
            const SequenceRow sequence_row = {pattern.sequence_index, i};
            pattern_rows.insert(pattern_row);
            pattern_rows_by_sequence_row[sequence_row].insert(pattern_row);
            if (selection.start == j) {
                selection_starts.insert(sequence_row);
            }
        }
    }
}

void GUIPatternsPanel::mark_selected_commands_pattern_rows(const size_t channel_index, const size_t pattern_id, const int row) {
    if (!selection.command) {
        return;
    }

    const CommandsPattern &pattern = current_patterns.commands_patterns[channel_index][pattern_id];
    for (int i = 0; i < pattern.commands.size(); ++i) {
        const int j = row + i;
        const PatternRow pattern_row = {channel_index, pattern_id, i};
        if (selection.is_row_selected(channel_index, j) ||
            (!selection.is_active() && pattern.current_row == i)) {
            const SequenceRow sequence_row = {pattern.sequence_index, i};
            pattern_rows.insert(pattern_row);
            pattern_rows_by_sequence_row[sequence_row].insert(pattern_row);
            if (selection.start == j) {
                selection_starts.insert(sequence_row);
            }
        }
    }
}

void GUIPatternsPanel::prepare_secondary_selection() {
    std::set<size_t> sequences;
    std::set<SequenceRow> sequence_rows;
    for (const auto &[sequence_row, pattern_rows] : pattern_rows_by_sequence_row) {
        sequences.insert(sequence_row.sequence_index);
        sequence_rows.insert(sequence_row);
    }

    secondary_pattern_rows.clear();
    secondary_sequence_rows.clear();
    for (const SequenceRow &sequence_row : sequence_rows) {
        const uint8_t sequence_index = sequence_row.sequence_index;
        if (sequences.find(sequence_index) == sequences.end()) {
            continue;
        }

        if (selection.command) {
            for (const auto &[channel_index, commands_patterns] : current_patterns.commands_patterns) {
                for (size_t pattern_id = 0; pattern_id < commands_patterns.size(); ++pattern_id) {
                    const CommandsPattern &commands_pattern = commands_patterns[pattern_id];
                    if (commands_pattern.sequence_index == sequence_index) {
                        const PatternRow pattern_row = {channel_index, pattern_id, sequence_row.row};
                        pattern_rows_by_sequence_row[sequence_row].insert(pattern_row);
                        secondary_pattern_rows.insert(pattern_row);
                        secondary_sequence_rows.insert(sequence_row);
                    }
                }
            }
        } else {
            for (const auto &[channel_index, patterns] : current_patterns.patterns) {
                for (size_t pattern_id = 0; pattern_id < patterns.size(); ++pattern_id) {
                    const Pattern &pattern = patterns[pattern_id];
                    if (pattern.sequence_index == sequence_index) {
                        const PatternRow pattern_row = {channel_index, pattern_id, sequence_row.row};
                        pattern_rows_by_sequence_row[sequence_row].insert(pattern_row);
                        secondary_pattern_rows.insert(pattern_row);
                        secondary_sequence_rows.insert(sequence_row);
                    }
                }
            }
        }
    }
}

PatternIndex GUIPatternsPanel::find_pattern_by_current_row() const {
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
    return {pattern, pattern_id, rows};
}

CommandsPatternIndex GUIPatternsPanel::find_commands_pattern_by_current_row() const {
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
    return {pattern, pattern_id, rows};
}

void GUIPatternsPanel::perform_notes_action(const std::string &action_name, const PatternSelectionChange<uint8_t> &changes) {
    const SetItemsFunction<uint8_t> function = [this](const std::map<PatternRow, uint8_t> &notes) {
        return this->set_notes(notes);
    };

    perform_action_pattern_selection<uint8_t>(this, {Target::SEQUENCE}, action_name, changes, function);
}

void GUIPatternsPanel::perform_commands_action(const std::string &action_name, const PatternSelectionChange<CommandValue> &changes) {
    const SetItemsFunction<CommandValue> function = [this](const std::map<PatternRow, CommandValue> &commands_changes) {
        return this->set_commands(commands_changes);
    };

    perform_action_pattern_selection<CommandValue>(this, {Target::COMMANDS_SEQUENCE}, action_name, changes, function);
}

int GUIPatternsPanel::get_pages() const {
    return std::ceil(static_cast<float>(current_patterns.total_rows) / gui.get_page_size());
}

void GUIPatternsPanel::deselect_all_rows() {
    for (auto &patterns : current_patterns.patterns) {
        for (auto &pattern : patterns.second) {
            pattern.current_row = -1;
        }
    }
    for (auto &commands_patterns : current_patterns.commands_patterns) {
        for (auto &commands_pattern : commands_patterns.second) {
            commands_pattern.current_row = -1;
            commands_pattern.selection = CommandSelection::None;
        }
    }
}

void GUIPatternsPanel::set_notes(const std::map<PatternRow, uint8_t> &notes) {
    for (const auto &[pattern_row, note] : notes) {
        set_note(pattern_row, note);
    }
}

void GUIPatternsPanel::set_note(const PatternRow &pattern_row, const uint8_t note) {
    set_note(pattern_row.channel_index, pattern_row.pattern_id, pattern_row.row, note);
}

void GUIPatternsPanel::set_commands(const std::map<PatternRow, CommandValue> &commands_values) {
    for (const auto &[pattern_row, command_value] : commands_values) {
        set_command(pattern_row, command_value);
    }
}

void GUIPatternsPanel::set_note(const size_t channel_index, const size_t pattern_id, const int row, const uint8_t note) {
    if (current_patterns.patterns.find(channel_index) != current_patterns.patterns.end()) {
        Pattern &pattern = current_patterns.patterns[channel_index][pattern_id];
        pattern.set_note(row, note);
        current_channel = {false, channel_index};
        current_row = pattern.starting_row + row;
    }
}

void GUIPatternsPanel::set_command(const PatternRow &pattern_row, const CommandValue &command_value) {
    set_command(pattern_row, command_value.first, command_value.second);
}

void GUIPatternsPanel::set_command(const PatternRow &pattern_row, const std::string &command, const std::string &value) {
    set_command(pattern_row.channel_index, pattern_row.pattern_id, pattern_row.row, command, value);
}

void GUIPatternsPanel::set_command(const size_t channel_index, const size_t pattern_id, const int row, const std::string &command, const std::string &value) {
    if (current_patterns.commands_patterns.find(channel_index) != current_patterns.commands_patterns.end()) {
        CommandsPattern &pattern = current_patterns.commands_patterns[channel_index][pattern_id];
        pattern.set_command(row, command, value);
        current_channel = {true, channel_index};
        current_row = pattern.starting_row + row;
    }
}

void GUIPatternsPanel::set_command(const size_t channel_index, const size_t pattern_id, const int row, const CommandValue &command_value) {
    set_command(channel_index, pattern_id, row, command_value.first, command_value.second);
}

int GUIPatternsPanel::get_current_page() const {
    return page;
}

int GUIPatternsPanel::get_current_row() const {
    return current_row;
}

bool GUIPatternsPanel::is_playing() const {
    return gui.is_playing() && ticks_per_beat > 0;
}

bool GUIPatternsPanel::is_active() const {
    return visible && ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
}

bool GUIPatternsPanel::is_commands_view_active() const {
    return is_active() && current_channel.command;
}

void GUIPatternsPanel::pre_actions() {
    prepare_secondary_selection();
}

void GUIPatternsPanel::post_actions() {
    selection_action = PatternSelectionAction::None;
}

void GUIPatternsPanel::register_shortcuts() {
    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeUp,
        [this]() {
            selection_action = PatternSelectionAction::TransposeUp;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeDown,
        [this]() {
            selection_action = PatternSelectionAction::TransposeDown;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeOctaveUp,
        [this]() {
            selection_action = PatternSelectionAction::TransposeOctaveUp;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternTransposeOctaveDown,
        [this]() {
            selection_action = PatternSelectionAction::TransposeOctaveDown;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectAll,
        [this]() {
            selection_action = PatternSelectionAction::SelectAll;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectChannel,
        [this]() {
            selection_action = PatternSelectionAction::SelectChannel;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSelectNone,
        [this]() {
            selection_action = PatternSelectionAction::DeselectAll;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditDelete,
        [this]() {
            selection_action = PatternSelectionAction::Delete;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditCut,
        [this]() {
            selection_action = PatternSelectionAction::Cut;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditCopy,
        [this]() {
            selection_action = PatternSelectionAction::Copy;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::EditPaste,
        [this]() {
            selection_action = PatternSelectionAction::Paste;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSetNoteRest,
        [this]() {
            selection_action = PatternSelectionAction::SetNoteRest;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSetNoteCut,
        [this]() {
            selection_action = PatternSelectionAction::SetNoteCut;
        }
    );

    shortcut_manager.register_shortcut(
        ShortcutAction::PatternSetNoteOff,
        [this]() {
            selection_action = PatternSelectionAction::SetNoteOff;
        }
    );
}
