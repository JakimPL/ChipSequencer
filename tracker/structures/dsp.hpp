#ifndef STRUCTURES_DSP_HPP
#define STRUCTURES_DSP_HPP

#include <cstddef>
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
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint8_t pad[8];
};

struct DSPGainer {
    uint8_t dsp_size = SIZE_DSP_GAINER;
    uint8_t effect_index = EFFECT_GAINER;
    uint8_t output_flag;
    uint8_t _unused;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint16_t volume;
    uint8_t pad[6];
};

struct DSPDistortion {
    uint8_t dsp_size = SIZE_DSP_DISTORTION;
    uint8_t effect_index = EFFECT_DISTORTION;
    uint8_t output_flag;
    uint8_t _unused;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint16_t level;
    uint8_t pad[6];
};

struct DSPFilter {
    uint8_t dsp_size = SIZE_DSP_FILTER;
    uint8_t effect_index = EFFECT_FILTER;
    uint8_t output_flag;
    uint8_t _unused;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint16_t frequency;
    uint8_t mode;
    uint8_t pad[5];
};

struct DSPDelay {
    uint8_t dsp_size = SIZE_DSP_DELAY;
    uint8_t effect_index = EFFECT_DELAY;
    uint8_t output_flag;
    uint8_t _unused;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint8_t dry;
    uint8_t wet;
    uint8_t feedback;
    uint16_t delay_time;
    uint8_t pad[3];
};

typedef std::vector<void *> DSPs;

constexpr size_t DSP_SIZE = offsetof(DSP, dsp_size);
constexpr size_t DSP_EFFECT_INDEX = offsetof(DSP, effect_index);
constexpr size_t DSP_OUTPUT_FLAG = offsetof(DSP, output_flag);
constexpr size_t DSP_OUTPUT = offsetof(DSP, output);
constexpr size_t DSP_SPLITTER = offsetof(DSP, splitter);
constexpr size_t DSP_GAINER_VOLUME = offsetof(DSPGainer, volume);
constexpr size_t DSP_DISTORTION_LEVEL = offsetof(DSPDistortion, level);
constexpr size_t DSP_FILTER_FREQUENCY = offsetof(DSPFilter, frequency);
constexpr size_t DSP_FILTER_MODE = offsetof(DSPFilter, mode);
constexpr size_t DSP_DELAY_DRY = offsetof(DSPDelay, dry);
constexpr size_t DSP_DELAY_WET = offsetof(DSPDelay, wet);
constexpr size_t DSP_DELAY_FEEDBACK = offsetof(DSPDelay, feedback);
constexpr size_t DSP_DELAY_TIME = offsetof(DSPDelay, delay_time);

#endif // STRUCTURES_DSP_HPP
