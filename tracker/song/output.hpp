#ifndef SONG_OUTPUT_HPP
#define SONG_OUTPUT_HPP

#include <cstdint>

#include "../maps/routing.hpp"
#include "links/link.hpp"

enum OutputTarget {
    OUTPUT_TARGET_OUTPUT = 0,
    OUTPUT_TARGET_DSP = 1,
    OUTPUT_TARGET_PARAMETER = 2,
};

struct OutputType {
    bool additive = true;
    int shift = 0;
    int variable_type;
    int target;

    int output_channel;
    int dsp_channel;

    int parameter_type;
    int routing_item;
    int routing_index;

    int index;
    int offset;

    uint8_t calculate_output_flag() const;
    void from_output_flag(const uint8_t output_flag);
    void from_link(const Link &link);
    void set_link(Link &link, const ItemType type, const uint8_t id) const;
};

#endif // SONG_OUTPUT_HPP
