#ifndef DSP_HPP
#define DSP_HPP

#include <cstdint>
#include <vector>

struct DSP {
    uint8_t dsp_size;
    uint8_t effect_index;
    void *output;
    uint8_t output_flag;
};

struct DSPDelay {
    uint8_t dsp_size = DSP_DELAY_SIZE;
    uint8_t effect_index = EFFECT_DELAY;
    void *output;
    uint8_t output_flag;
    uint16_t dry;
    uint16_t wet;
    uint16_t feedback;
    uint16_t delay_time;
};

struct DSPGainer {
    uint8_t dsp_size = DSP_GAINER_SIZE;
    uint8_t effect_index = EFFECT_GAINER;
    void *output;
    uint8_t output_flag;
    uint16_t volume;
};

struct DSPFilter {
    uint8_t dsp_size = DSP_FILTER_SIZE;
    uint8_t effect_index = EFFECT_FILTER;
    void *output;
    uint8_t output_flag;
    uint16_t frequency;
};

typedef std::vector<void *> DSPs;

#endif // DSP_HPP
