#ifndef STRUCTURES_WAVETABLE_HPP
#define STRUCTURES_WAVETABLE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#include "../constants.hpp"

typedef std::array<uint8_t, MAX_WAVETABLE_SIZE> WavetableData;

struct Wavetable {
    uint16_t wavetable_size;
    WavetableData data;

    void serialize(std::ofstream &file) const;
    static Wavetable *deserialize(std::ifstream &file);
} __attribute__((packed));

typedef std::vector<Wavetable *> Wavetables;

constexpr size_t WAVETABLE_SIZE = offsetof(Wavetable, wavetable_size);
constexpr size_t WAVETABLE_DATA = offsetof(Wavetable, data);

#endif // STRUCTURES_WAVETABLE_HPP
