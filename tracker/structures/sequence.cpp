#include "../utils/file.hpp"
#include "sequence.hpp"

void Sequence::serialize(std::ofstream &file) const {
    write_data(file, &data_size, 1);
    for (size_t i = 0; i < data_size / 2; i++) {
        write_data(file, &notes[i], sizeof(Note));
    }
}