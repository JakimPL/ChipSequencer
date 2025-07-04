#pragma once

#include <array>
#include <cstdint>

#include "../structures/dsp.hpp"
#include "../structures/sizes.hpp"

template <typename... T>
constexpr std::array<uint8_t, sizeof...(T)> make_dsps_sizes(T... args) {
    static_assert(sizeof...(T) == static_cast<size_t>(Effect::Count), "Number of DSP sizes must match Effect::Count");
    return {static_cast<uint8_t>(args)...};
}

constexpr auto dsps_sizes = make_dsps_sizes(
    SIZE_DSP_GAINER,
    SIZE_DSP_DISTORTION,
    SIZE_DSP_FILTER,
    SIZE_DSP_DELAY
);
