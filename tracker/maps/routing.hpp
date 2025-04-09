#ifndef MAPS_ROUTING_HPP
#define MAPS_ROUTING_HPP

#include <map>
#include <string>
#include <vector>

#include "../constants.hpp"
#include "../song/target.hpp"

struct RoutingItems {
    std::vector<std::string> labels;
    std::vector<uint16_t> offsets;
    std::vector<TargetVariableType> types;
};

const std::map<Target, RoutingItems> routing_variables = {
    {
        Target::ENVELOPE,
        RoutingItems({
            {
                "Base volume",
                "Sustain level",
                "Attack",
                "Decay",
                "Hold",
                "Release",
            },
            {
                ENVELOPE_BASE_VOLUME,
                ENVELOPE_SUSTAIN_LEVEL,
                ENVELOPE_ATTACK,
                ENVELOPE_DECAY,
                ENVELOPE_HOLD,
                ENVELOPE_RELEASE,
            },
            {
                TargetVariableType::Int16,
                TargetVariableType::Int16,
                TargetVariableType::Int16,
                TargetVariableType::Int16,
                TargetVariableType::Int16,
                TargetVariableType::Int16,
            },
        }),
    },
    {
        Target::SEQUENCE,
        RoutingItems(),
    },
    {
        Target::ORDER,
        RoutingItems(),
    },
    {
        Target::OSCILLATOR,
        RoutingItems({
            {
                "Duty cycle",
                "Wavetable index",
            },
            {
                OSCILLATOR_SQUARE_DUTY_CYCLE,
                OSCILLATOR_WAVETABLE_WAVETABLE_INDEX,
            },
            {
                TargetVariableType::Int8,
                TargetVariableType::Int8,
            },
        }),
    },
    {
        Target::WAVETABLE,
        RoutingItems(),
    },
    {
        Target::DSP,
        RoutingItems({
            {
                "Gain",
                "Dry",
                "Wet",
                "Feedback",
                "Delay time",
                "Cutoff frequency",
            },
            {
                DSP_GAINER_VOLUME,
                DSP_DELAY_DRY,
                DSP_DELAY_WET,
                DSP_DELAY_FEEDBACK,
                DSP_DELAY_TIME,
                DSP_FILTER_FREQUENCY,
            },
            {
                TargetVariableType::Int16,
                TargetVariableType::Int16,
                TargetVariableType::Int16,
                TargetVariableType::Int16,
                TargetVariableType::Int16,
                TargetVariableType::Int16,
            },
        }),
    },
    {
        Target::CHANNEL,
        RoutingItems({
            {
                "Pitch",
            },
            {
                CHANNEL_PITCH,
            },
            {
                TargetVariableType::Int16,
            },
        }),
    },
};

#endif // MAPS_ROUTING_HPP
