#ifndef SONG_VALIDATION_HPP
#define SONG_VALIDATION_HPP

enum class ValidationResult {
    OK,
    InvalidSongLength,
    OscillatorMissingWavetable,
    OrderMissingSequence,
    ChannelMissingOscillator,
    ChannelMissingEnvelope,
    ChannelMissingOrder,
};

#endif // SONG_VALIDATION_HPP
