#ifndef SONG_LINKS_TARGET_HPP
#define SONG_LINKS_TARGET_HPP

#include <cstdint>

enum class Target : uint8_t {
    OUTPUT_CHANNEL,
    DSP_CHANNEL,
    ENVELOPE,
    SEQUENCE,
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
