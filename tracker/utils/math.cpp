#include "math.hpp"

int mod(int a, int b) {
    return (a % b + b) % b;
}

void set_flag(uint8_t &flag, const uint8_t bit, const bool value) {
    if (value) {
        flag |= bit;
    } else {
        flag &= static_cast<uint8_t>(~bit);
    }
}

void combine_hash(size_t &seed, const size_t value) {
    seed ^= value + 0x9e3779b9 + (seed << 6U) + (seed >> 2U);
}

void combine_hash(size_t &seed, const std::string &string) {
    combine_hash(seed, std::hash<std::string>{}(string));
}
