#ifndef DSP_HPP
#define DSP_HPP

#include <cstdint>

struct DSP {
    uint8_t dsp_size;
    uint8_t effect_index;
    _Float32 *output;
    uint8_t output_flag;
};

struct DSPDelay {
    uint8_t dsp_size = DSP_DELAY_SIZE;
    uint8_t effect_index = EFFECT_DELAY;
    _Float32 *output;
    uint8_t output_flag;
    uint16_t dry;
    uint16_t wet;
    uint16_t feedback;
    uint16_t delay_time;
};

struct DSPGainer {
    uint8_t dsp_size = DSP_GAINER_SIZE;
    uint8_t effect_index = EFFECT_GAINER;
    _Float32 *output;
    uint8_t output_flag;
    uint16_t volume;
};

struct DSPFilter {
    uint8_t dsp_size = DSP_FILTER_SIZE;
    uint8_t effect_index = EFFECT_FILTER;
    _Float32 *output;
    uint8_t output_flag;
    uint16_t frequency;
};

#endif // DSP_HPP
