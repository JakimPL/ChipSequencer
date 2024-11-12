#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

#include <cstdint>
#include <vector>
#include <utility>

struct Sequence {
    std::vector<std::pair<uint8_t, uint8_t>> notes;
};

#endif // SEQUENCE_HPP
