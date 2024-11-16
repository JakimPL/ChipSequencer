#ifndef WAVETABLE_HPP
#define WAVETABLE_HPP

#include <cstdint>
#include <vector>

struct Wavetable {
    uint8_t wavetable_size;
    uint8_t wavetable[];
};

typedef std::vector<Wavetable *> Wavetables;

#endif // WAVETABLE_HPP
