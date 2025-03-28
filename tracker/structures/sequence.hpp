#ifndef STRUCTURES_SEQUENCE_HPP
#define STRUCTURES_SEQUENCE_HPP

#include <cstdint>
#include <fstream>
#include <vector>

struct Note {
    int8_t pitch;
    uint8_t duration;
};

struct Sequence {
    uint8_t data_size;
    Note notes[];

    void serialize(std::ofstream &file) const;
};

typedef std::vector<Sequence *> Sequences;

#endif // STRUCTURES_SEQUENCE_HPP
