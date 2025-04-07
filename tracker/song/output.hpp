#ifndef SONG_OUTPUT_HPP
#define SONG_OUTPUT_HPP

#include <cstdint>

struct OutputType {
    bool additive;
    int variable_type;
    int shift;

    uint8_t calculate_output_flag() const;
    void from_output_flag(const uint8_t output_flag);
};

#endif // SONG_OUTPUT_HPP
