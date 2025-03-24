#ifndef STRUCTURES_WAVETABLE_HPP
#define STRUCTURES_WAVETABLE_HPP

#include <cstdint>
#include <vector>

#include "../utils.hpp"

struct Wavetable {
    uint8_t wavetable_size;
    uint8_t wavetable[];

    void serialize(std::ofstream &file) const {
        write_data(file, &wavetable_size, 1);
        for (size_t i = 0; i < wavetable_size; i++) {
            write_data(file, &wavetable[i], sizeof(uint8_t));
        }
    }
};

typedef std::vector<Wavetable *> Wavetables;

#endif // STRUCTURES_WAVETABLE_HPP
