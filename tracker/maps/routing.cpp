#include "routing.hpp"

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

RoutingItems::RoutingItems(std::vector<RoutingItem> items) {
    for (size_t i = 0; i < items.size(); ++i) {
        const auto &item = items[i];
        labels.push_back(item.label);
        offsets.push_back(item.offset);
        types.push_back(item.type);
        constraints.push_back(item.constraint);
        offset_to_index[item.offset] = i;
    }
}

RoutingTuple RoutingItems::filter_items(const int index) const {
    std::vector<size_t> indices;
    std::vector<std::string> filtered_labels;
    std::vector<uint16_t> filtered_offsets;
    std::vector<TargetVariableType> filtered_types;
    for (size_t i = 0; i < labels.size(); ++i) {
        if (constraints[i] == -1 || constraints[i] == index) {
            indices.push_back(i);
            filtered_labels.push_back(labels[i]);
            filtered_offsets.push_back(offsets[i]);
            filtered_types.push_back(types[i]);
        }
    }

    return {indices, filtered_labels, filtered_offsets, filtered_types};
}
