#ifndef STRUCTURES_OSCILLATOR_HPP
#define STRUCTURES_OSCILLATOR_HPP

#include <cstdint>
#include <vector>

#include "../constants.hpp"

struct Oscillator {
    uint8_t oscillator_size;
    uint8_t generator_index;
    uint8_t pad[2];
};

struct OscillatorSquare {
    uint8_t oscillator_size = SIZE_OSCILLATOR_SQUARE;
    uint8_t generator_index = GENERATOR_SQUARE;
    uint8_t duty_cycle;
    uint8_t pad[1];
};

struct OscillatorSaw {
    uint8_t oscillator_size = SIZE_OSCILLATOR_SAW;
    uint8_t generator_index = GENERATOR_SAW;
    uint8_t reverse;
    uint8_t pad[1];
};

struct OscillatorSine {
    uint8_t oscillator_size = SIZE_OSCILLATOR_SINE;
    uint8_t generator_index = GENERATOR_SINE;
    uint8_t pad[2];
};

struct OscillatorWavetable {
    uint8_t oscillator_size = SIZE_OSCILLATOR_WAVETABLE;
    uint8_t generator_index = GENERATOR_WAVETABLE;
    uint8_t wavetable_index;
    uint8_t interpolation;
};

struct OscillatorNoise {
    uint8_t oscillator_size = SIZE_OSCILLATOR_NOISE;
    uint8_t generator_index = GENERATOR_NOISE;
    uint8_t pad[2];
};

typedef std::vector<void *> Oscillators;

#endif // STRUCTURES_OSCILLATOR_HPP
