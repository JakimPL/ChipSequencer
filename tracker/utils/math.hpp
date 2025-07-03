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

void combine_hash(size_t &seed, size_t value);
void combine_hash(size_t &seed, const std::string &string);
