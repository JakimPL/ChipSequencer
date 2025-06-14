#pragma once

enum class ValidationResult {
    OK,
    InvalidSongLength,
    OscillatorMissingWavetable,
    OrderMissingSequence,
    ChannelMissingOscillator,
    ChannelMissingEnvelope,
    ChannelMissingOrder,
};
