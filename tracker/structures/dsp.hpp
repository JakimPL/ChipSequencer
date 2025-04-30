#ifndef STRUCTURES_DSP_HPP
#define STRUCTURES_DSP_HPP

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#include "../constants.hpp"

struct DSP {
    uint8_t dsp_size = SIZE_DSP;
    uint8_t effect_index;
    uint8_t output_flag;
    uint8_t flag;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint8_t pad[0];
    uint32_t alignment[2];
};

struct DSPGainer {
    uint8_t dsp_size = SIZE_DSP_GAINER;
    uint8_t effect_index = EFFECT_GAINER;
    uint8_t output_flag;
    uint8_t flag;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint16_t volume;
    uint8_t pad[2];
    uint32_t alignment[1];
};

struct DSPDistortion {
    uint8_t dsp_size = SIZE_DSP_DISTORTION;
    uint8_t effect_index = EFFECT_DISTORTION;
    uint8_t output_flag;
    uint8_t flag;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint16_t level;
    uint8_t pad[2];
    uint32_t alignment[1];
};

struct DSPFilter {
    uint8_t dsp_size = SIZE_DSP_FILTER;
    uint8_t effect_index = EFFECT_FILTER;
    uint8_t output_flag;
    uint8_t flag;
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint16_t frequency;
    uint8_t mode;
    uint8_t pad[1];
    uint32_t alignment[1];
};

struct DSPDelay {
    uint8_t dsp_size = SIZE_DSP_DELAY;
    uint8_t effect_index = EFFECT_DELAY;
    uint8_t output_flag;
    uint8_t flag;
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

/* padding is necessary for 32-bit systems */
static_assert(sizeof(DSP) == SIZE_DSP + 1 + 2 * sizeof(uint32_t), "DSP must be of 16 bytes.");
static_assert(sizeof(DSPGainer) == SIZE_DSP_GAINER + 1 + sizeof(uint32_t), "DSPGainer must be of 16 bytes.");
static_assert(sizeof(DSPDistortion) == SIZE_DSP_DISTORTION + 1 + sizeof(uint32_t), "DSPDistortion must be of 16 bytes.");
static_assert(sizeof(DSPFilter) == SIZE_DSP_FILTER + 1 + sizeof(uint32_t), "DSPFilter must be of 16 bytes.");
static_assert(sizeof(DSPDelay) == SIZE_DSP_DELAY + 1, "DSPDelay must be of 20 bytes.");

static_assert(sizeof(DSP) == sizeof(DSPGainer), "DSP and DSPGainer must be of the same size.");
static_assert(sizeof(DSP) == sizeof(DSPDistortion), "DSP and DSPDistortion must be of the same size.");
static_assert(sizeof(DSP) == sizeof(DSPFilter), "DSP and DSPFilter must be of the same size.");
static_assert(sizeof(DSP) == sizeof(DSPDelay), "DSP and DSPDelay must be of the same size.");

#endif // STRUCTURES_DSP_HPP
