#ifndef STRUCTURES_SEQUENCE_HPP
#define STRUCTURES_SEQUENCE_HPP

#include <cstdint>
#include <fstream>
#include <vector>

#include "../utils.hpp"

struct Note {
    int8_t pitch;
    uint8_t duration;
};

struct Sequence {
    uint8_t data_size;
    Note notes[];

    void serialize(std::ofstream &file) const {
        write_data(file, &data_size, 1);
        for (size_t i = 0; i < data_size / 2; i++) {
            write_data(file, &notes[i], sizeof(Note));
        }
    }
};

typedef std::vector<Sequence *> Sequences;

#endif // STRUCTURES_SEQUENCE_HPP
