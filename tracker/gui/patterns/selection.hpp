#ifndef GUI_PATTERNS_SELECTION_HPP
#define GUI_PATTERNS_SELECTION_HPP

#include <cstddef>
#include <set>
#include <utility>

#include "pattern.hpp"

typedef std::set<std::pair<Pattern *, int>> PatternRows;

void transpose(const PatternRows &pattern_rows, const int value);

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
