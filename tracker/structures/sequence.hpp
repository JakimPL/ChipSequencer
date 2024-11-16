#ifndef SEQUENCE_HPP
#define SEQUENCE_HPP

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

#endif // SEQUENCE_HPP
