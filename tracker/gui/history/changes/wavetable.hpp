#pragma once

#include <cstdint>
#include <vector>

#include "../../constants.hpp"

struct WavetableChange {
    int &wavetable_size;
    std::vector<float> &wavetable;
    std::vector<float> old_wave;
    std::vector<float> new_wave;

    WavetableChange(int &size, std::vector<float> &wt, const std::vector<float> &old_w)
        : wavetable_size(size), wavetable(wt), old_wave(old_w), new_wave(wt) {}
};
