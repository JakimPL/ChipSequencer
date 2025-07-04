#pragma once

#include <map>
#include <vector>

#include "../constants.hpp"
#include "../utils.hpp"
#include "../clipboard/items/commands.hpp"
#include "../clipboard/items/notes.hpp"
#include "../history/actions/selection.hpp"
#include "../patterns/commands.hpp"
#include "../patterns/display.hpp"
#include "../patterns/index.hpp"
#include "../patterns/pattern.hpp"
#include "../patterns/selection.hpp"
#include "panel.hpp"

class GUIPatternsPanel : public GUIPanel {
  private:
    struct CurrentPatterns {
        uint16_t total_rows = 0;
        std::map<size_t, std::vector<Pattern>> patterns;
        std::map<size_t, std::vector<CommandsPattern>> commands_patterns;
        std::map<size_t, uint16_t> patterns_max_rows;
        std::map<size_t, uint16_t> commands_patterns_max_rows;
        std::map<VariantChannelIndex, int> playing_rows;
    } current_patterns;

    int page = 0;
    VariantChannelIndex current_channel = {false, 0};
    PatternSelection selection;
    PatternSelectionAction selection_action = PatternSelectionAction::None;
    PatternRows pattern_rows;
    PatternRows secondary_pattern_rows;
    SequenceRows selection_starts;
    SequenceRows secondary_sequence_rows;
    PatternRowsBySequenceRow pattern_rows_by_sequence_row;
    size_t current_index = -1;
    int current_row = -1;

    void draw_pages();
    void draw_channels();
    void draw_channel(size_t channel_index);
    void draw_commands_channel(size_t channel_index);

    void select_channel();
    void select_all();
    void deselect_all();
    void set_selection_note(uint8_t note);
    void copy_selection();
    void paste_selection();
    void delete_selection();
    void transpose_selected_rows(int value);
    void prepare_secondary_selection();

    void paste_pattern_selection(ClipboardNotes *notes);
    void paste_commands_pattern_selection(ClipboardCommands *commands);

    void mark_selected_rows(bool command, const PatternRow &pattern_row);
    void mark_selected_rows(bool command, size_t channel_index, size_t pattern_id, int row);
    void mark_selected_pattern_rows(size_t channel_index, size_t pattern_id, int row);
    void mark_selected_commands_pattern_rows(size_t channel_index, size_t pattern_id, int row);

    PatternIndex find_pattern_by_current_row() const;
    CommandsPatternIndex find_commands_pattern_by_current_row() const;

    void perform_notes_action(const std::string &action_name, const PatternSelectionChange<uint8_t> &changes);
    void perform_commands_action(const std::string &action_name, const PatternSelectionChange<CommandValue> &changes);
    void perform_note_action(uint8_t old_note, uint8_t new_note, const PatternRow &pattern_row, uint8_t sequence_index, int index);
    void perform_command_action(const CommandValue &old_command, const CommandValue &new_command, const PatternRow &pattern_row, uint8_t sequence_index, int index);

    void process_sequence(
        size_t channel_index,
        size_t j,
        uint8_t sequence_index,
        uint16_t &row
    );

    void process_commands_sequence(
        size_t channel_index,
        size_t j,
        uint8_t sequence_index,
        uint16_t &row
    );

    void handle_pattern_input(Pattern *pattern, uint16_t index);
    void handle_commands_pattern_input(CommandsPattern *pattern, uint16_t index);
    int get_pages() const;
    bool is_playing() const;
    bool is_commands_view() const;

    void clear();
    void add_repeated_patterns();
    void add_repeated_commands_patterns();
    void from_sequences();
    void from_commands_sequences();
    void to_sequences() const;
    void to_commands_sequences() const;

    void register_shortcuts() override;
    void draw() override;
    void check_keyboard_input() override;
    void shortcut_actions() override;
    void pre_actions() override;
    void post_actions() override;

  public:
    explicit GUIPatternsPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void set_notes(const std::map<PatternRow, uint8_t> &notes);
    void set_note(const PatternRow &pattern_row, uint8_t note);
    void set_note(size_t channel_index, size_t pattern_id, int row, uint8_t note);

    void set_commands(const std::map<PatternRow, CommandValue> &commands_values);
    void set_command(const PatternRow &pattern_row, const std::string &command, const std::string &value);
    void set_command(const PatternRow &pattern_row, const CommandValue &command_value);
    void set_command(size_t channel_index, size_t pattern_id, int row, const std::string &command, const std::string &value);
    void set_command(size_t channel_index, size_t pattern_id, int row, const CommandValue &command_value);

    void from() override;
    void to() const override;

    void deselect_all_rows();
    bool is_active() const;
    bool is_commands_view_active() const;
    int get_current_row() const;
    int get_current_page() const;
};
