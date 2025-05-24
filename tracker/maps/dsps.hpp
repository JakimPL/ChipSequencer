#ifndef MAPS_DSPS_HPP
#define MAPS_DSPS_HPP

#include <array>
#include <cstdint>

#include "../sizes.hpp"
#include "../structures/dsp.hpp"

const std::array<uint8_t, static_cast<size_t>(Effect::Count)> dsps_sizes = {
    SIZE_DSP_GAINER,
    SIZE_DSP_DISTORTION,
    SIZE_DSP_FILTER,
    SIZE_DSP_DELAY,
};

#endif // MAPS_DSPS_HPP
