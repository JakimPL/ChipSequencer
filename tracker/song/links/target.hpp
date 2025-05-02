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
    COMMANDS,
    ORDER,
    OSCILLATOR,
    WAVETABLE,
    DSP,
    CHANNEL,
    UNUSED
};

enum class TargetVariableType : uint8_t {
    Float,
    Int8,
    Int16,
    Int32
};

#endif // SONG_LINKS_TARGET_HPP
