#ifndef STRUCTURES_WAVETABLE_HPP
#define STRUCTURES_WAVETABLE_HPP

#include <cstdint>
#include <vector>

struct Wavetable {
    uint8_t wavetable_size;
    uint8_t wavetable[];
};

typedef std::vector<Wavetable *> Wavetables;

#endif // STRUCTURES_WAVETABLE_HPP
