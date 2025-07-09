#pragma once

#include <cstddef>
#include <vector>

struct SequenceChange {
    size_t sequence_index;
    std::vector<uint8_t> old_sequence;
    std::vector<uint8_t> new_sequence;

    SequenceChange(size_t index, const std::vector<uint8_t> &old_seq, const std::vector<uint8_t> &new_seq)
        : sequence_index(index), old_sequence(old_seq), new_sequence(new_seq) {}
};
