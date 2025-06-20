#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

struct Note {
    uint8_t pitch;
    uint8_t duration;
} __attribute__((packed));

typedef std::array<Note, UINT8_MAX> Notes;

struct Sequence {
    uint8_t size;
    Notes notes;

    void from_note_vector(const std::vector<Note> &note_vector);
    void serialize(std::ofstream &file) const;
    static Sequence *deserialize(std::ifstream &file);
} __attribute__((packed));

typedef std::vector<Sequence *> Sequences;

constexpr size_t SEQUENCE_SIZE = offsetof(Sequence, size);
constexpr size_t SEQUENCE_NOTES = offsetof(Sequence, notes);
