#ifndef SONG_OUTPUT_HPP
#define SONG_OUTPUT_HPP

#include <cstdint>

#include "../maps/routing.hpp"
#include "../structures/channel.hpp"
#include "links/link.hpp"

enum class OutputTarget {
    OutputSplitter,
    DSPSplitter,
    DirectOutput,
    DirectDSP,
    Parameter,
};

enum class OutputOperation {
    Add = 0b00,
    Set = 0b01,
    Multiply = 0b10,
    Zero = 0b11,
};

struct OutputType {
    int operation = static_cast<int>(OutputOperation::Add);
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

    bool splitter_on = true;
    std::array<float, MAX_OUTPUT_CHANNELS> splitter = {0.5f, 0.5f, 0.0f, 0.0f};

    uint8_t calculate_output_flag() const;
    void from_output_flag(const uint8_t output_flag);
    void from_link(const Link &link);
    void set_link(Link &link, const ItemType type, const uint8_t id) const;

    void load_splitter(const uint8_t target[], const Link &link);
    void set_splitter(uint8_t target[]) const;
    uint32_t get_splitter_data() const;
    static std::array<uint8_t, MAX_OUTPUT_CHANNELS> unpack_splitter_data(uint32_t splitter_data);
};

#endif // SONG_OUTPUT_HPP
