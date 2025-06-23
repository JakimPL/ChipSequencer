#pragma once

#include <cstdint>
#include <vector>

#include "../../constants.hpp"

struct WavetableChange {
    std::vector<float> &wavetable;
    std::vector<float> old_wave;
    std::vector<float> new_wave;
};
