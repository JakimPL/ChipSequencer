#ifndef STRUCTURES_DSP_HPP
#define STRUCTURES_DSP_HPP

#include <cstdint>
#include <fstream>
#include <vector>

#include "../constants.hpp"

struct DSP {
    uint8_t dsp_size;
    uint8_t effect_index;
    uint8_t output_flag;
    uint8_t _unused;
    void *output;
    uint8_t pad[8];
};

struct DSPGainer {
    uint8_t dsp_size = SIZE_DSP_GAINER;
    uint8_t effect_index = EFFECT_GAINER;
    uint8_t output_flag;
    uint8_t _unused;
    void *output;
    uint16_t volume;
    uint8_t pad[6];
};

struct DSPDelay {
    uint8_t dsp_size = SIZE_DSP_DELAY;
    uint8_t effect_index = EFFECT_DELAY;
    uint8_t output_flag;
    uint8_t _unused;
    void *output;
    uint16_t dry;
    uint16_t wet;
    uint16_t feedback;
    uint16_t delay_time;
};

struct DSPFilter {
    uint8_t dsp_size = SIZE_DSP_FILTER;
    uint8_t effect_index = EFFECT_FILTER;
    uint8_t output_flag;
    uint8_t _unused;
    void *output;
    uint16_t frequency;
    uint8_t mode;
    uint8_t pad[5];
};

typedef std::vector<void *> DSPs;

#endif // STRUCTURES_DSP_HPP
