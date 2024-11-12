#ifndef DSP_HPP
#define DSP_HPP

#include <cstdint>

struct DSP {
    uint16_t output;
    uint8_t output_flag;

    virtual ~DSP() = default;
};

struct EffectDelay : public DSP {
    uint16_t dry;
    uint16_t wet;
    uint16_t feedback;
    uint16_t delay_time;
};

struct EffectGainer : public DSP {
    uint16_t volume;
};

struct EffectFilter : public DSP {
    uint16_t frequency;
};

#endif // DSP_HPP
