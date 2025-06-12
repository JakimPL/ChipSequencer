#ifndef GUI_PATTERNS_SELECTION_HPP
#define GUI_PATTERNS_SELECTION_HPP

#include <cstddef>
#include <set>
#include <utility>

#include "pattern.hpp"

struct PatternRow {
    size_t channel_index;
    size_t pattern_id;
    int row;

    bool operator<(const PatternRow &other) const {
        return std::tie(channel_index, pattern_id, row) < std::tie(other.channel_index, other.pattern_id, other.row);
    }
};

struct SequenceRow {
    size_t sequence_index;
    int row;

    bool operator<(const SequenceRow &other) const {
        return std::tie(sequence_index, row) < std::tie(other.sequence_index, other.row);
    }
};

typedef std::set<PatternRow> PatternRows;
typedef std::map<SequenceRow, std::set<PatternRow>> PatternRowsBySequenceRow;

struct PatternSelection {
    bool selecting = false;
    size_t channel_index = -1;
    int start = -1;
    int end = -1;
    int channel_start = -1;
    int channel_end = -1;

    bool is_row_selected(const size_t channel_index, const int row) const;
    void form(const size_t channel_index, const int row);
    void reset();
    bool is_active() const;
};

#endif // GUI_PATTERNS_SELECTION_HPP
