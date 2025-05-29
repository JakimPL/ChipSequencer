#include "routing.hpp"

const std::map<Target, RoutingItems> routing_variables = {
    {
        Target::ENVELOPE,
        RoutingItems({
            {"Base volume", ENVELOPE_BASE_VOLUME, TargetVariableType::Word},
            {"Sustain level", ENVELOPE_SUSTAIN_LEVEL, TargetVariableType::Word},
            {"Attack", ENVELOPE_ATTACK, TargetVariableType::Word},
            {"Decay", ENVELOPE_DECAY, TargetVariableType::Word},
            {"Hold", ENVELOPE_HOLD, TargetVariableType::Word},
            {"Release", ENVELOPE_RELEASE, TargetVariableType::Word},
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
            {"Duty cycle", OSCILLATOR_SQUARE_DUTY_CYCLE, TargetVariableType::Byte, GENERATOR_SQUARE},
            {"Reverse", OSCILLATOR_SAW_REVERSE, TargetVariableType::Byte, GENERATOR_SAW},
            {"Interpolation", OSCILLATOR_WAVETABLE_INTERPOLATION, TargetVariableType::Byte, GENERATOR_WAVETABLE},
        }),
    },
    {
        Target::WAVETABLE,
        RoutingItems({}),
    },
    {
        Target::DSP,
        RoutingItems({
            {"Gain", DSP_GAINER_VOLUME, TargetVariableType::Word, EFFECT_GAINER},
            {"Level", DSP_DISTORTION_LEVEL, TargetVariableType::Word, EFFECT_DISTORTION},
            {"Cutoff frequency", DSP_FILTER_FREQUENCY, TargetVariableType::Word, EFFECT_FILTER},
            {"High-pass", DSP_FILTER_MODE, TargetVariableType::Byte, EFFECT_FILTER},
            {"Dry", DSP_DELAY_DRY, TargetVariableType::Byte, EFFECT_DELAY},
            {"Wet", DSP_DELAY_WET, TargetVariableType::Byte, EFFECT_DELAY},
            {"Feedback", DSP_DELAY_FEEDBACK, TargetVariableType::Byte, EFFECT_DELAY},
            {"Delay time", DSP_DELAY_TIME, TargetVariableType::Word, EFFECT_DELAY},
            {"Splitter 0", DSP_SPLITTER, TargetVariableType::Byte},
            {"Splitter 1", DSP_SPLITTER + 1, TargetVariableType::Byte},
            {"Splitter 2", DSP_SPLITTER + 2, TargetVariableType::Byte},
            {"Splitter 3", DSP_SPLITTER + 3, TargetVariableType::Byte},
        }),
    },
    {
        Target::CHANNEL,
        RoutingItems({
            {"Envelope index", CHANNEL_ENVELOPE_INDEX, TargetVariableType::Byte},
            {"Oscillator index", CHANNEL_OSCILLATOR_INDEX, TargetVariableType::Byte},
            {"Order index", CHANNEL_ORDER_INDEX, TargetVariableType::Byte},
            {"Pitch", CHANNEL_PITCH, TargetVariableType::Dword},
            {"Splitter 0", CHANNEL_SPLITTER, TargetVariableType::Byte},
            {"Splitter 1", CHANNEL_SPLITTER + 1, TargetVariableType::Byte},
            {"Splitter 2", CHANNEL_SPLITTER + 2, TargetVariableType::Byte},
            {"Splitter 3", CHANNEL_SPLITTER + 3, TargetVariableType::Byte},
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
