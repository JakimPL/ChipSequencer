#pragma once

#include <map>
#include <vector>

#include "../constants.hpp"
#include "../utils.hpp"
#include "../patterns/commands.hpp"
#include "../patterns/pattern.hpp"
#include "../patterns/selection.hpp"
#include "panel.hpp"

struct VariantChannelIndex {
    bool command;
    size_t index;

    bool operator<(const VariantChannelIndex &other) const {
        return std::tie(command, index) < std::tie(other.command, other.index);
    }
};

struct PatternIndex {
    Pattern *pattern;
    size_t pattern_id;
    uint16_t index;

    bool operator<(const PatternIndex &other) const {
        return std::tie(pattern_id, index) < std::tie(other.pattern_id, other.index);
    }
};

struct CommandsPatternIndex {
    CommandsPattern *pattern;
    size_t pattern_id;
    uint16_t index;

    bool operator<(const CommandsPatternIndex &other) const {
        return std::tie(pattern_id, index) < std::tie(other.pattern_id, other.index);
    }
};

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
    void set_selection_note(const uint8_t note);
    void delete_selection();
    void transpose_selected_rows(const int value);
    void prepare_secondary_selection();
    void mark_selected_rows(const bool command, const size_t channel_index, const size_t pattern_id, const int row);
    void mark_selected_pattern_rows(const size_t channel_index, const size_t pattern_id, const int row);
    void mark_selected_commands_pattern_rows(const size_t channel_index, const size_t pattern_id, const int row);
    PatternIndex find_pattern_by_current_row() const;
    CommandsPatternIndex find_commands_pattern_by_current_row() const;

    void handle_pattern_input(Pattern *pattern, uint16_t index);
    void handle_commands_pattern_input(CommandsPattern *pattern, uint16_t index);
    int get_pages() const;
    bool is_playing() const;

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
    GUIPatternsPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;

    void set_note(const size_t channel_index, const size_t pattern_id, const int row, const uint8_t note);

    void from() override;
    void to() const override;

    void deselect_all_rows();
    bool is_active() const;
    bool is_commands_view_active() const;
};
