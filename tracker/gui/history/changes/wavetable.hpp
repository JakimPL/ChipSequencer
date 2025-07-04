#pragma once

#include <cstdint>
#include <vector>
#include <utility>

#include "../../constants.hpp"

struct WavetableChange {
    int &wavetable_size;
    std::vector<float> &wavetable;
    std::vector<float> old_wave;
    std::vector<float> new_wave;

    WavetableChange(int &size, std::vector<float> &wt, std::vector<float> old)
        : wavetable_size(size), wavetable(wt), old_wave(std::move(old)), new_wave(wt) {}
};
