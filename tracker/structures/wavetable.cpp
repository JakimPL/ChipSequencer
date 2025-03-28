#include "../utils/file.hpp"
#include "wavetable.hpp"

void Wavetable::serialize(std::ofstream &file) const {
    write_data(file, &wavetable_size, 1);
    for (size_t i = 0; i < wavetable_size; i++) {
        write_data(file, &wavetable[i], sizeof(uint8_t));
    }
}
