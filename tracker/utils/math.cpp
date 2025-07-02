#include "math.hpp"

int mod(int a, int b) {
    return (a % b + b) % b;
}

void set_flag(uint8_t &flag, const uint8_t bit, const bool value) {
    if (value) {
        flag |= bit;
    } else {
        flag &= ~bit;
    }
}

void combine_hash(std::size_t &seed, std::size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

template <typename T>
void combine_hash(std::size_t &seed, const T &value) {
    combine_hash(seed, std::hash<T>{}(value));
}

template void combine_hash(std::size_t &seed, const uint8_t &value);
