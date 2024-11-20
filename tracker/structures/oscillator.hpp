#ifndef STRUCTURES_OSCILLATOR_HPP
#define STRUCTURES_OSCILLATOR_HPP

#include <cstdint>
#include <vector>

struct Oscillator {
    uint8_t oscillator_size;
    uint8_t generator_index;
};

struct OscillatorSquare {
    uint8_t oscillator_size = OSCILLATOR_SQUARE_SIZE;
    uint8_t generator_index = OSCILLATOR_SQUARE;
    uint8_t duty_cycle;
};

struct OscillatorSaw {
    uint8_t oscillator_size = OSCILLATOR_SAW_SIZE;
    uint8_t generator_index = OSCILLATOR_SAW;
};

struct OscillatorSine {
    uint8_t oscillator_size = OSCILLATOR_SINE_SIZE;
    uint8_t generator_index = OSCILLATOR_SINE;
};

struct OscillatorWavetable {
    uint8_t oscillator_size = OSCILLATOR_WAVETABLE_SIZE;
    uint8_t generator_index = OSCILLATOR_WAVETABLE;
    uint8_t wavetable_index;
};

typedef std::vector<void *> Oscillators;

#endif // STRUCTURES_OSCILLATOR_HPP
