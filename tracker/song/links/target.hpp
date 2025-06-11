#ifndef SONG_LINKS_TARGET_HPP
#define SONG_LINKS_TARGET_HPP

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
    COUNT,
};

enum class TargetVariableType : uint8_t {
    Float,
    Byte,
    Word,
    Dword,
    Count,
};

bool is_target_splitter(const Target target);
bool is_target_output(const Target target);
bool is_target_parameter(const Target target);

#endif // SONG_LINKS_TARGET_HPP
