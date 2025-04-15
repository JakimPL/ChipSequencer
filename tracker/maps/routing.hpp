#ifndef MAPS_ROUTING_HPP
#define MAPS_ROUTING_HPP

#include <map>
#include <string>
#include <vector>

#include "../constants.hpp"
#include "../song/links/target.hpp"

struct RoutingItems {
    RoutingItems(
        std::vector<std::string> labels = {},
        std::vector<uint16_t> offsets = {},
        std::vector<TargetVariableType> types = {},
        std::vector<int> constraints = {}
    );

    std::vector<std::string> labels;
    std::vector<uint16_t> offsets;
    std::vector<TargetVariableType> types;
    std::vector<int> constraints;
    std::map<uint16_t, size_t> offset_to_index;

    std::pair<std::vector<size_t>, std::vector<std::string>> filter_items(const int index) const;
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
            {
                -1,
                -1,
                -1,
                -1,
                -1,
                -1,
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
            },
            {
                OSCILLATOR_SQUARE_DUTY_CYCLE,
            },
            {
                TargetVariableType::Int8,
            },
            {
                GENERATOR_SQUARE,
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
            {
                EFFECT_GAINER,
                EFFECT_DELAY,
                EFFECT_DELAY,
                EFFECT_DELAY,
                EFFECT_DELAY,
                EFFECT_FILTER,
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
                TargetVariableType::Int32,
            },
            {
                -1,
            },
        }),
    },
};

#endif // MAPS_ROUTING_HPP
