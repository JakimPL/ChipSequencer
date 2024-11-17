#ifndef STRUCTURES_SEQUENCE_HPP
#define STRUCTURES_SEQUENCE_HPP

#include <cstdint>
#include <vector>

struct Note {
    int8_t pitch;
    uint8_t duration;
};

struct Sequence {
    uint8_t data_size;
    Note notes[];
};

typedef std::vector<Sequence *> Sequences;

#endif // STRUCTURES_SEQUENCE_HPP
