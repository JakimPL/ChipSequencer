#ifndef MAPS_OSCILLATORS_HPP
#define MAPS_OSCILLATORS_HPP

#include <array>
#include <cstdint>

#include "../sizes.hpp"
#include "../structures/oscillator.hpp"

template <typename... T>
constexpr std::array<uint8_t, sizeof...(T)> make_oscillators_sizes(T... args) {
    static_assert(sizeof...(T) == static_cast<size_t>(Generator::Count), "Number of oscillator sizes must match Generator::Count");
    return {static_cast<uint8_t>(args)...};
}

constexpr auto oscillators_sizes = make_oscillators_sizes(
    SIZE_OSCILLATOR_SQUARE,
    SIZE_OSCILLATOR_SAW,
    SIZE_OSCILLATOR_SINE,
    SIZE_OSCILLATOR_WAVETABLE,
    SIZE_OSCILLATOR_NOISE
);

#endif // MAPS_OSCILLATORS_HPP
