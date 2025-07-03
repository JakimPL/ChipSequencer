#pragma once

#include <cstdint>

enum class Target : uint8_t {
    SPLITTER_OUTPUT,
    SPLITTER_DSP,
    DIRECT_OUTPUT,
    DIRECT_DSP,
    ENVELOPE,
    SEQUENCE,
    COMMANDS_SEQUENCE,
    ORDER,
    OSCILLATOR,
    WAVETABLE,
    DSP,
    CHANNEL,
    COMMANDS_CHANNEL,
    SPECIAL,
    COUNT,
};

enum class TargetVariableType : uint8_t {
    Float,
    Byte,
    Word,
    Dword,
    Count,
};

bool is_target_splitter(Target target);
bool is_target_output(Target target);
bool is_target_parameter(Target target);
