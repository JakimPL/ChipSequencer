#ifndef SONG_OUTPUT_HPP
#define SONG_OUTPUT_HPP

#include <cstdint>

enum OutputTarget {
    OUTPUT_TARGET_OUTPUT = 0,
    OUTPUT_TARGET_DSP = 1,
    OUTPUT_TARGET_PARAMETER = 2,
};

struct OutputType {
    bool additive;
    int variable_type;
    int shift;
    int target;

    uint8_t calculate_output_flag() const;
    void from_output_flag(const uint8_t output_flag);
};

#endif // SONG_OUTPUT_HPP
