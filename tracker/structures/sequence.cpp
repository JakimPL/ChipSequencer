#include "../general.hpp"
#include "../utils/file.hpp"
#include "sequence.hpp"

void Sequence::from_note_vector(const std::vector<Note> &note_vector) {
    const size_t length = note_vector.size();
    const uint8_t data_size = static_cast<uint8_t>(length * sizeof(Note));
    size = data_size;
    std::copy(note_vector.begin(), note_vector.end(), this->notes.begin());
}

void Sequence::serialize(std::ofstream &file) const {
    write_data(file, &size, 1);
    for (size_t i = 0; i < size / 2; i++) {
        write_data(file, &notes[i], sizeof(Note));
    }
}

Sequence *Sequence::deserialize(std::ifstream &file) {
    if (!file.good()) {
        throw std::runtime_error("Sequence file stream is not good");
    }

    uint8_t size;
    read_data(file, &size, sizeof(size));

    Sequence *sequence = resource_manager.allocate<Sequence>();
    sequence->size = size;

    for (size_t i = 0; i < size / 2; i++) {
        read_data(file, &sequence->notes[i], sizeof(Note));
    }

    return sequence;
}
