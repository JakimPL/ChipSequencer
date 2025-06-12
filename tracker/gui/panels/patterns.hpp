#ifndef GUI_PANELS_PATTERNS_HPP
#define GUI_PANELS_PATTERNS_HPP

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
    PatternRows pattern_rows;
    PatternRows secondary_pattern_rows;
    PatternRowsBySequenceRow pattern_rows_by_sequence_row;
    size_t current_index = -1;
    int current_row = -1;
    int transpose_by = 0;

    void draw_pages();
    void draw_channels();
    void draw_channel(size_t channel_index);
    void draw_commands_channel(size_t channel_index);

    void transpose_selected_rows();
    void prepare_secondary_selection();
    void mark_selected_rows(const size_t channel_index, const size_t pattern_id, const int row);
    std::pair<Pattern *, uint16_t> find_pattern_by_current_row() const;
    std::pair<CommandsPattern *, uint16_t> find_commands_pattern_by_current_row() const;

    void handle_pattern_input(Pattern *pattern, uint16_t index);
    void handle_commands_pattern_input(CommandsPattern *pattern, uint16_t index);
    int get_pages() const;
    bool is_playing() const;

    void from_sequences();
    void from_commands_sequences();
    void to_sequences() const;
    void to_commands_sequences() const;

    void add() override {};
    void duplicate() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIPatternsPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;

    void from() override;
    void to() const override;

    void deselect_all_rows();
};

#endif // GUI_PANELS_PATTERNS_HPP
