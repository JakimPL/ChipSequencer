#ifndef STRUCTURES_WAVETABLE_HPP
#define STRUCTURES_WAVETABLE_HPP

#include <array>
#include <cstdint>
#include <fstream>
#include <vector>

typedef std::array<uint8_t, UINT8_MAX> WavetableData;

struct Wavetable {
    uint8_t wavetable_size;
    WavetableData data;

    void serialize(std::ofstream &file) const;
    static Wavetable *deserialize(std::ifstream &file);
};

typedef std::vector<Wavetable *> Wavetables;

#endif // STRUCTURES_WAVETABLE_HPP
