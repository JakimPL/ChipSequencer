#include "../utils/file.hpp"
#include "sequence.hpp"

void Sequence::from_note_vector(const std::vector<Note> &note_vector) {
    const size_t length = note_vector.size();
    const uint8_t data_size = static_cast<uint8_t>(length * sizeof(Note));
    this->data_size = data_size;
    std::copy(note_vector.begin(), note_vector.end(), this->notes.begin());
}

void Sequence::serialize(std::ofstream &file) const {
    write_data(file, &data_size, 1);
    for (size_t i = 0; i < data_size / 2; i++) {
        write_data(file, &notes[i], sizeof(Note));
    }
}

Sequence *Sequence::deserialize(std::ifstream &file) {
    if (!file.good()) {
        throw std::runtime_error("Sequence file stream is not good");
    }

    uint8_t data_size;
    read_data(file, &data_size, sizeof(data_size));

    void *memory = malloc(sizeof(Sequence));
    Sequence *sequence = static_cast<Sequence *>(memory);
    sequence->data_size = data_size;

    for (size_t i = 0; i < data_size / 2; i++) {
        read_data(file, &sequence->notes[i], sizeof(Note));
    }

    return sequence;
}
