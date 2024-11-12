#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include <cstdint>

struct Oscillator {
    uint8_t generator_index;

    virtual ~Oscillator() = default;
};

struct OscillatorSquare : public Oscillator {
    uint8_t duty_cycle;
};

struct OscillatorWavetable : public Oscillator {
    uint8_t wavetable_index;
};

#endif // OSCILLATOR_HPP
