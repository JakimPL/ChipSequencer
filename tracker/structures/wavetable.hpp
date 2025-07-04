#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#include "../constants.hpp"

using WavetableData = std::array<uint8_t, MAX_WAVETABLE_SIZE>;

struct Wavetable {
    uint16_t wavetable_size;
    WavetableData data;

    void serialize(std::ofstream &file) const;
    static Wavetable *deserialize(std::ifstream &file);
} __attribute__((packed));

using Wavetables = std::vector<Wavetable *>;

constexpr size_t WAVETABLE_SIZE = offsetof(Wavetable, wavetable_size);
constexpr size_t WAVETABLE_DATA = offsetof(Wavetable, data);
