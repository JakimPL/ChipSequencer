#pragma once

#include <cstddef>

struct OrderSequenceChange {
    size_t sequence_index;
    size_t old_sequence;
    size_t new_sequence;
};
