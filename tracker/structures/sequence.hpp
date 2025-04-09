#ifndef STRUCTURES_SEQUENCE_HPP
#define STRUCTURES_SEQUENCE_HPP

#include <array>
#include <cstdint>
#include <fstream>
#include <vector>

struct Note {
    uint8_t pitch;
    uint8_t duration;
};

typedef std::array<Note, UINT8_MAX> Notes;

struct Sequence {
    uint8_t data_size;
    Notes notes;

    void from_note_vector(const std::vector<Note> &note_vector);
    void serialize(std::ofstream &file) const;
    static Sequence *deserialize(std::ifstream &file);
};

typedef std::vector<Sequence *> Sequences;

#endif // STRUCTURES_SEQUENCE_HPP
