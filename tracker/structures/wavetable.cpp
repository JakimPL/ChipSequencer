#include "../utils/file.hpp"
#include "wavetable.hpp"

void Wavetable::serialize(std::ofstream &file) const {
    write_data(file, &wavetable_size, 1);
    for (size_t i = 0; i < wavetable_size; i++) {
        write_data(file, &wavetable[i], sizeof(uint8_t));
    }
}

Wavetable *Wavetable::deserialize(std::ifstream &file) {
    if (!file.good()) {
        throw std::runtime_error("Wavetable file stream is not good");
    }

    uint8_t wavetable_size;
    read_data(file, &wavetable_size, sizeof(wavetable_size));

    size_t totalSize = (wavetable_size + 1) * sizeof(uint8_t);
    void *memory = malloc(totalSize);
    Wavetable *wavetable = static_cast<Wavetable *>(memory);
    wavetable->wavetable_size = wavetable_size;

    for (size_t i = 0; i < wavetable_size; i++) {
        read_data(file, &wavetable->wavetable[i], sizeof(uint8_t));
    }

    return wavetable;
}
