#ifndef OSCILLATOR_HPP
#define OSCILLATOR_HPP

#include <cstdint>

struct Oscillator {
    uint8_t oscillator_size;
    uint8_t generator_index;
};

struct OscillatorSquare {
    uint8_t oscillator_size = 2;
    uint8_t generator_index = OSCILLATOR_SQUARE;
    uint8_t duty_cycle;
};

struct OscillatorSaw {
    uint8_t oscillator_size = 1;
    uint8_t generator_index = OSCILLATOR_SAW;
};

struct OscillatorSine {
    uint8_t oscillator_size = 1;
    uint8_t generator_index = OSCILLATOR_SINE;
};

struct OscillatorWavetable {
    uint8_t oscillator_size = 2;
    uint8_t generator_index = OSCILLATOR_WAVETABLE;
    uint8_t wavetable_index;
};

#endif // OSCILLATOR_HPP
