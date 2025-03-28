#ifndef STRUCTURES_WAVETABLE_HPP
#define STRUCTURES_WAVETABLE_HPP

#include <cstdint>
#include <fstream>
#include <vector>

struct Wavetable {
    uint8_t wavetable_size;
    uint8_t wavetable[];

    void serialize(std::ofstream &file) const;
};

typedef std::vector<Wavetable *> Wavetables;

#endif // STRUCTURES_WAVETABLE_HPP
