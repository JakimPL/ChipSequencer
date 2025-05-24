#ifndef MAPS_OSCILLATORS_HPP
#define MAPS_OSCILLATORS_HPP

#include <array>
#include <cstdint>

#include "../sizes.hpp"
#include "../structures/oscillator.hpp"

const std::array<uint8_t, static_cast<size_t>(Generator::Count)> oscillators_sizes = {
    SIZE_OSCILLATOR_SQUARE,
    SIZE_OSCILLATOR_SAW,
    SIZE_OSCILLATOR_SINE,
    SIZE_OSCILLATOR_WAVETABLE,
    SIZE_OSCILLATOR_NOISE,
};

#endif // MAPS_OSCILLATORS_HPP
