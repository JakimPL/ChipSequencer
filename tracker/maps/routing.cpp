#include <sstream>

#include "../constants.hpp"
#include "../structures.hpp"
#include "routing.hpp"

extern std::vector<DSP *> dsps;
extern std::vector<Oscillator *> oscillators;

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
        Target::COMMANDS_SEQUENCE,
        RoutingItems({}),
    },
    {
        Target::ORDER,
        RoutingItems({
            {"Order length", ORDER_LENGTH, TargetVariableType::Byte, ROUTING_HIDDEN},
        }),
    },
    {
        Target::OSCILLATOR,
        RoutingItems({
            {"Duty cycle", OSCILLATOR_SQUARE_DUTY_CYCLE, TargetVariableType::Word, GENERATOR_SQUARE},
            {"Reverse", OSCILLATOR_SAW_REVERSE, TargetVariableType::Byte, GENERATOR_SAW},
            {"Interpolation", OSCILLATOR_WAVETABLE_INTERPOLATION, TargetVariableType::Byte, GENERATOR_WAVETABLE},
        }),
    },
    {
        Target::WAVETABLE,
        RoutingItems({
            {"Wavetable size", WAVETABLE_SIZE, TargetVariableType::Word, ROUTING_HIDDEN},
        }),
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
            {"Envelope index", CHANNEL_ENVELOPE_INDEX, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Oscillator index", CHANNEL_OSCILLATOR_INDEX, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Order index", CHANNEL_ORDER_INDEX, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Pitch", CHANNEL_PITCH, TargetVariableType::Dword},
            {"Splitter 0", CHANNEL_SPLITTER, TargetVariableType::Byte},
            {"Splitter 1", CHANNEL_SPLITTER + 1, TargetVariableType::Byte},
            {"Splitter 2", CHANNEL_SPLITTER + 2, TargetVariableType::Byte},
            {"Splitter 3", CHANNEL_SPLITTER + 3, TargetVariableType::Byte},
        }),
    },
    {
        Target::COMMANDS_CHANNEL,
        RoutingItems({
            {"Order index", COMMANDS_CHANNEL_ORDER_INDEX, TargetVariableType::Byte, ROUTING_HIDDEN},
        }),
    },
    {
        Target::SPECIAL,
        RoutingItems({
            {"BPM", SPECIAL_BPM, TargetVariableType::Word, ROUTING_HIDDEN},
            {"Division", SPECIAL_DIVISION, TargetVariableType::Word, ROUTING_HIDDEN},
            {"Master gainer", SPECIAL_MASTER_GAINER, TargetVariableType::Float, ROUTING_HIDDEN},
            {"Sample rate", SPECIAL_SAMPLE_RATE, TargetVariableType::Dword, ROUTING_HIDDEN},
            {"A4 frequency", SPECIAL_A4_FREQUENCY, TargetVariableType::Word, ROUTING_HIDDEN},
            {"EDO", SPECIAL_EDO, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Output channels", SPECIAL_OUTPUT_CHANNELS, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Shift", SPECIAL_SHIFT, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Channel index", SPECIAL_CHANNEL_INDEX, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"DSP index", SPECIAL_DSP_INDEX, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Hide channel", SPECIAL_CHANNEL_HIDE, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Bypass channel", SPECIAL_CHANNEL_BYPASS, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Constant pitch", SPECIAL_CHANNEL_CONSTANT_PITCH, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Synchronize", SPECIAL_CHANNEL_SYNCHRONIZE, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Sync numerator", SPECIAL_CHANNEL_SYNC_NUMERATOR, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Sync denominator", SPECIAL_CHANNEL_SYNC_DENOMINATOR, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Bypass DSP", SPECIAL_DSP_BYPASS, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Bypass command channel", SPECIAL_COMMAND_CHANNEL_BYPASS, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Hide command channel", SPECIAL_COMMAND_CHANNEL_HIDE, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Sequence length", SPECIAL_SEQUENCE_LENGTH, TargetVariableType::Byte, ROUTING_HIDDEN},
            {"Commands sequence length", SPECIAL_COMMANDS_SEQUENCE_LENGTH, TargetVariableType::Byte, ROUTING_HIDDEN},
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
        offset_to_index[{item.constraint, item.offset}] = i;
    }
}

RoutingTuple RoutingItems::filter_items(const int constraint, const bool allow_hidden) const {
    std::vector<size_t> indices;
    std::vector<std::string> filtered_labels;
    std::vector<uint16_t> filtered_offsets;
    std::vector<TargetVariableType> filtered_types;
    for (size_t i = 0; i < labels.size(); ++i) {
        if (constraints[i] == ROUTING_NO_CONSTRAINTS ||
            constraints[i] == constraint ||
            (allow_hidden && constraints[i] == ROUTING_HIDDEN)) {
            indices.push_back(i);
            filtered_labels.push_back(labels[i]);
            filtered_offsets.push_back(offsets[i]);
            filtered_types.push_back(types[i]);
        }
    }

    return {indices, filtered_labels, filtered_offsets, filtered_types};
}

size_t RoutingItems::get_index_from_offset(const LinkKey key) const {
    if (offset_to_index.empty()) {
        return -1;
    }

    int constraint = ROUTING_NO_CONSTRAINTS;
    if (key.target == Target::DSP) {
        const DSP *dsp = static_cast<DSP *>(dsps[key.index]);
        constraint = dsp->effect_index;
    } else if (key.target == Target::OSCILLATOR) {
        const Oscillator *oscillator = static_cast<Oscillator *>(oscillators[key.index]);
        constraint = oscillator->generator_index;
    }

    for (const auto &[key_constraint, index] : offset_to_index) {
        const auto &[item_constraint, item_offset] = key_constraint;
        if (key.offset == item_offset) {
            if (constraint == ROUTING_NO_CONSTRAINTS ||
                constraint == item_constraint) {
                return index;
            }
        }
    }

    return -1;
}

std::string get_key_name(const LinkKey key) {
    if (key.target == Target::COUNT || key.index < 0) {
        return "Unknown";
    }

    const RoutingItems &items = routing_variables.at(key.target);
    size_t index = items.get_index_from_offset(key);
    if (index == -1 || index >= items.labels.size()) {
        return "Unknown";
    }

    std::ostringstream stream;
    stream << items.labels[index] << " "
           << key.index;

    return stream.str();
}
