#ifndef MAPS_ROUTING_HPP
#define MAPS_ROUTING_HPP

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "../constants.hpp"
#include "../structures.hpp"
#include "../song/links/target.hpp"

typedef std::tuple<std::vector<size_t>, std::vector<std::string>, std::vector<uint16_t>, std::vector<TargetVariableType>> RoutingTuple;

struct RoutingItem {
    std::string label;
    uint16_t offset;
    TargetVariableType type;
    int constraint = -1;
    int default_shift = 0;
};

struct RoutingItems {
    RoutingItems(std::vector<RoutingItem> items);

    std::vector<std::string> labels;
    std::vector<uint16_t> offsets;
    std::vector<TargetVariableType> types;
    std::vector<int> constraints;
    std::map<uint16_t, size_t> offset_to_index;

    RoutingTuple filter_items(const int index) const;
};

const std::map<Target, RoutingItems> routing_variables = {
    {
        Target::ENVELOPE,
        RoutingItems({
            {"Base volume", ENVELOPE_BASE_VOLUME, TargetVariableType::Int16},
            {"Sustain level", ENVELOPE_SUSTAIN_LEVEL, TargetVariableType::Int16},
            {"Attack", ENVELOPE_ATTACK, TargetVariableType::Int16},
            {"Decay", ENVELOPE_DECAY, TargetVariableType::Int16},
            {"Hold", ENVELOPE_HOLD, TargetVariableType::Int16},
            {"Release", ENVELOPE_RELEASE, TargetVariableType::Int16},
        }),
    },
    {
        Target::SEQUENCE,
        RoutingItems({}),
    },
    {
        Target::ORDER,
        RoutingItems({}),
    },
    {
        Target::OSCILLATOR,
        RoutingItems({
            {"Duty cycle", OSCILLATOR_SQUARE_DUTY_CYCLE, TargetVariableType::Int8, GENERATOR_SQUARE},
            {"Interpolation", OSCILLATOR_WAVETABLE_INTERPOLATION, TargetVariableType::Int8, GENERATOR_WAVETABLE},
        }),
    },
    {
        Target::WAVETABLE,
        RoutingItems({}),
    },
    {
        Target::DSP,
        RoutingItems({
            {"Gain", DSP_GAINER_VOLUME, TargetVariableType::Int16, EFFECT_GAINER},
            {"Level", DSP_DISTORTION_LEVEL, TargetVariableType::Int16, EFFECT_DISTORTION},
            {"Cutoff frequency", DSP_FILTER_FREQUENCY, TargetVariableType::Int16, EFFECT_FILTER},
            {"High-pass", DSP_FILTER_MODE, TargetVariableType::Int8, EFFECT_FILTER},
            {"Dry", DSP_DELAY_DRY, TargetVariableType::Int8, EFFECT_DELAY},
            {"Wet", DSP_DELAY_WET, TargetVariableType::Int8, EFFECT_DELAY},
            {"Feedback", DSP_DELAY_FEEDBACK, TargetVariableType::Int8, EFFECT_DELAY},
            {"Delay time", DSP_DELAY_TIME, TargetVariableType::Int16, EFFECT_DELAY},
            {"Splitter 0", DSP_SPLITTER, TargetVariableType::Int8},
            {"Splitter 1", DSP_SPLITTER + 1, TargetVariableType::Int8},
            {"Splitter 2", DSP_SPLITTER + 2, TargetVariableType::Int8},
            {"Splitter 3", DSP_SPLITTER + 3, TargetVariableType::Int8},
        }),
    },
    {
        Target::CHANNEL,
        RoutingItems({
            {"Pitch", CHANNEL_PITCH, TargetVariableType::Int32},
            {"Splitter 0", CHANNEL_SPLITTER, TargetVariableType::Int8},
            {"Splitter 1", CHANNEL_SPLITTER + 1, TargetVariableType::Int8},
            {"Splitter 2", CHANNEL_SPLITTER + 2, TargetVariableType::Int8},
            {"Splitter 3", CHANNEL_SPLITTER + 3, TargetVariableType::Int8},
        }),
    },
};

#endif // MAPS_ROUTING_HPP
