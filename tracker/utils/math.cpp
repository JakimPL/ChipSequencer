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
