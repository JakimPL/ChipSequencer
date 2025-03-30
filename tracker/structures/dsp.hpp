#ifndef STRUCTURES_DSP_HPP
#define STRUCTURES_DSP_HPP

#include <cstdint>
#include <fstream>
#include <vector>

#include "../constants.hpp"

struct DSP {
    uint8_t dsp_size;
    uint8_t effect_index;
    void *output;
    uint8_t output_flag;
};

struct DSPDelay {
    uint8_t dsp_size = SIZE_DSP_DELAY;
    uint8_t effect_index = EFFECT_DELAY;
    void *output;
    uint8_t output_flag;
    uint16_t dry;
    uint16_t wet;
    uint16_t feedback;
    uint16_t delay_time;
};

struct DSPGainer {
    uint8_t dsp_size = SIZE_DSP_GAINER;
    uint8_t effect_index = EFFECT_GAINER;
    void *output;
    uint8_t output_flag;
    uint16_t volume;
};

struct DSPFilter {
    uint8_t dsp_size = SIZE_DSP_FILTER;
    uint8_t effect_index = EFFECT_FILTER;
    void *output;
    uint8_t output_flag;
    uint16_t frequency;
};

typedef std::vector<void *> DSPs;

#endif // STRUCTURES_DSP_HPP
