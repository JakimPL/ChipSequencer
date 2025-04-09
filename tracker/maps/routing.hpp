#ifndef MAPS_ROUTING_HPP
#define MAPS_ROUTING_HPP

#include <map>
#include <string>

#include "../constants.hpp"
#include "../song/target.hpp"

const std::map<Target, std::map<std::string, std::pair<uint8_t, TargetVariableType>>> routing_variables = {
    {
        Target::ENVELOPE,
        {
            {"Base volume", {ENVELOPE_BASE_VOLUME, TargetVariableType::Int16}},
            {"Sustain level", ENVELOPE_SUSTAIN_LEVEL, TargetVariableType::Int16},
            {"Attack", ENVELOPE_ATTACK, TargetVariableType::Int16},
            {"Decay", ENVELOPE_DECAY, TargetVariableType::Int16},
            {"Hold", ENVELOPE_HOLD, TargetVariableType::Int16},
            {"Release", ENVELOPE_RELEASE, TargetVariableType::Int16},
        },
    },
    {
        Target::OSCILLATOR,
        {
            {"Duty cycle", OSCILLATOR_SQUARE_DUTY_CYCLE, TargetVariableType::Int8},
            {"Wavetable index", OSCILLATOR_WAVETABLE_WAVETABLE_INDEX, TargetVariableType::Int8},
        },
    },
    {
        Target::DSP,
        {
            {"Gain", DSP_GAINER_VOLUME, TargetVariableType::Int16},
            {"Dry", DSP_DELAY_DRY, TargetVariableType::Int16},
            {"Wet", DSP_DELAY_WET, TargetVariableType::Int16},
            {"Feedback", DSP_DELAY_FEEDBACK, TargetVariableType::Int16},
            {"Delay time", DSP_DELAY_TIME, TargetVariableType::Int16},
            {"Cutoff frequency", DSP_FILTER_FREQUENCY, TargetVariableType::Int16},
        },
    },
    {
        Target::CHANNEL,
        {
            {"Pitch", CHANNEL_PITCH, TargetVariableType::Int16},
        },
    },
};

#endif // MAPS_ROUTING_HPP
