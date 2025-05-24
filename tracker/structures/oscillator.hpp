#ifndef STRUCTURES_OSCILLATOR_HPP
#define STRUCTURES_OSCILLATOR_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include "../constants.hpp"

enum class Generator : uint8_t {
    Square = GENERATOR_SQUARE,
    Saw = GENERATOR_SAW,
    Sine = GENERATOR_SINE,
    Wavetable = GENERATOR_WAVETABLE,
    Noise = GENERATOR_NOISE,
    Count = GENERATOR_COUNT,
};

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
    uint8_t pad[0];
};

struct OscillatorNoise {
    uint8_t oscillator_size = SIZE_OSCILLATOR_NOISE;
    uint8_t generator_index = GENERATOR_NOISE;
    uint8_t pad[2];
};

typedef std::vector<void *> Oscillators;

constexpr size_t OSCILLATOR_SIZE = offsetof(Oscillator, oscillator_size);
constexpr size_t OSCILLATOR_GENERATOR_INDEX = offsetof(Oscillator, generator_index);
constexpr size_t OSCILLATOR_SQUARE_DUTY_CYCLE = offsetof(OscillatorSquare, duty_cycle);
constexpr size_t OSCILLATOR_SAW_REVERSE = offsetof(OscillatorSaw, reverse);
constexpr size_t OSCILLATOR_WAVETABLE_WAVETABLE_INDEX = offsetof(OscillatorWavetable, wavetable_index);
constexpr size_t OSCILLATOR_WAVETABLE_INTERPOLATION = offsetof(OscillatorWavetable, interpolation);

static_assert(sizeof(OscillatorSine) == SIZE_OSCILLATOR_SINE + 1 + sizeof(OscillatorSine::pad), "OscillatorSine must be of 1 byte.");
static_assert(sizeof(OscillatorSquare) == SIZE_OSCILLATOR_SQUARE + 1 + sizeof(OscillatorSquare::pad), "OscillatorSquare must be of 2 bytes.");
static_assert(sizeof(OscillatorSaw) == SIZE_OSCILLATOR_SAW + 1 + sizeof(OscillatorSaw::pad), "OscillatorSaw must be of 2 bytes.");
static_assert(sizeof(OscillatorWavetable) == SIZE_OSCILLATOR_WAVETABLE + 1 + sizeof(OscillatorWavetable::pad), "OscillatorWavetable must be of 3 bytes.");
static_assert(sizeof(OscillatorNoise) == SIZE_OSCILLATOR_NOISE + 1 + sizeof(OscillatorNoise::pad), "OscillatorNoise must be of 1 byte.");

#endif // STRUCTURES_OSCILLATOR_HPP
