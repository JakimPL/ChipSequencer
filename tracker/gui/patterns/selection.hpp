#pragma once

#include <cstddef>
#include <map>
#include <set>
#include <utility>

#include "pattern.hpp"

struct PatternRow {
    size_t channel_index;
    size_t pattern_id;
    int row;

    bool operator==(const PatternRow &other) const {
        return channel_index == other.channel_index &&
               pattern_id == other.pattern_id &&
               row == other.row;
    }

    bool operator!=(const PatternRow &other) const {
        return !(*this == other);
    }

    bool operator<(const PatternRow &other) const {
        return std::tie(channel_index, pattern_id, row) < std::tie(other.channel_index, other.pattern_id, other.row);
    }
};

struct SequenceRow {
    size_t sequence_index;
    int row;

    bool operator==(const SequenceRow &other) const {
        return sequence_index == other.sequence_index && row == other.row;
    }

    bool operator!=(const SequenceRow &other) const {
        return !(*this == other);
    }

    bool operator<(const SequenceRow &other) const {
        return std::tie(sequence_index, row) < std::tie(other.sequence_index, other.row);
    }
};

typedef std::set<PatternRow> PatternRows;
typedef std::set<SequenceRow> SequenceRows;
typedef std::map<SequenceRow, std::set<PatternRow>> PatternRowsBySequenceRow;

struct PatternSelection {
    bool command = false;
    bool selecting = false;
    size_t channel_index = -1;
    int start = -1;
    int end = -1;
    int channel_start = -1;
    int channel_end = -1;

    void select(
        const int start,
        const int end,
        const bool command = false,
        const size_t channel_start = 0,
        const size_t channel_end = 0
    );
    bool is_row_selected(const size_t channel_index, const int row) const;
    void form(const bool command, const size_t channel_index, const int row);
    void clear();
    bool is_active() const;
    void adjust_selection();
};

enum class PatternSelectionAction {
    None,
    TransposeUp,
    TransposeDown,
    TransposeOctaveUp,
    TransposeOctaveDown,
    SelectAll,
    SelectChannel,
    DeselectAll,
    Delete,
    Cut,
    Copy,
    Paste,
    SetNoteRest,
    SetNoteCut,
    SetNoteOff,
};
