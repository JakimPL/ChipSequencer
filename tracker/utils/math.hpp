#pragma once

#include <algorithm>
#include <cstdint>

int mod(int a, int b);
void set_flag(uint8_t &flag, const uint8_t bit, const bool value);

template <typename T>
constexpr T clamp(const T &value, const T &min, const T &max) {
    if (min > max) {
        return max;
    }
    return std::clamp(value, min, max);
}

void combine_hash(std::size_t &seed, std::size_t value);

template <typename T>
void combine_hash(std::size_t &seed, const T &value);
