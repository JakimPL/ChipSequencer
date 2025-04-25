#ifndef STRUCTURES_SEQUENCE_HPP
#define STRUCTURES_SEQUENCE_HPP

#include <array>
#include <cstddef>
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

constexpr size_t SEQUENCE_DATA_SIZE = offsetof(Sequence, data_size);
constexpr size_t SEQUENCE_NOTES = offsetof(Sequence, notes);

#endif // STRUCTURES_SEQUENCE_HPP
