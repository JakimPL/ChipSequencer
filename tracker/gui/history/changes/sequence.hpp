#pragma once

#include <cstddef>
#include <vector>

template <typename T>
struct SequenceChange {
    size_t sequence_index;
    std::vector<T> old_sequence;
    std::vector<T> new_sequence;

    SequenceChange(size_t index, const std::vector<T> &old_seq, const std::vector<T> &new_seq)
        : sequence_index(index), old_sequence(old_seq), new_sequence(new_seq) {}
};
