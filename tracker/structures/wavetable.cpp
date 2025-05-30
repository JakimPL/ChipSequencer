#include "../utils/file.hpp"
#include "wavetable.hpp"

void Wavetable::serialize(std::ofstream &file) const {
    write_data(file, &wavetable_size, sizeof(wavetable_size));
    for (size_t i = 0; i < wavetable_size; i++) {
        write_data(file, &data[i], sizeof(uint8_t));
    }
}

Wavetable *Wavetable::deserialize(std::ifstream &file) {
    if (!file.good()) {
        throw std::runtime_error("Wavetable file stream is not good");
    }

    uint16_t wavetable_size;
    read_data(file, &wavetable_size, sizeof(wavetable_size));

    Wavetable *wavetable = new Wavetable;
    wavetable->wavetable_size = wavetable_size;

    for (size_t i = 0; i < wavetable_size; i++) {
        read_data(file, &wavetable->data[i], sizeof(uint8_t));
    }

    return wavetable;
}
