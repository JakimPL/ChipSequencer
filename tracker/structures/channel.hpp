#ifndef STRUCTURES_CHANNEL_HPP
#define STRUCTURES_CHANNEL_HPP

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#include "../constants.hpp"

struct Channel {
    void *output;
    uint8_t splitter[MAX_OUTPUT_CHANNELS];
    uint8_t output_flag;
    uint8_t envelope_index;
    uint8_t order_index;
    uint8_t oscillator_index;
    uint32_t pitch;

    void serialize_output(std::ofstream &file, const Channel *channel) const;
    void serialize_body(std::ofstream &file, const Channel *channel) const;
    static Channel *deserialize(std::ifstream &output_file, std::ifstream &body_file);
};

typedef std::vector<Channel *> Channels;

constexpr size_t CHANNEL_OUTPUT = offsetof(Channel, output);
constexpr size_t CHANNEL_SPLITTER = offsetof(Channel, splitter);
constexpr size_t CHANNEL_OUTPUT_FLAG = offsetof(Channel, output_flag);
constexpr size_t CHANNEL_ENVELOPE_INDEX = offsetof(Channel, envelope_index);
constexpr size_t CHANNEL_ORDER_INDEX = offsetof(Channel, order_index);
constexpr size_t CHANNEL_OSCILLATOR_INDEX = offsetof(Channel, oscillator_index);
constexpr size_t CHANNEL_PITCH = offsetof(Channel, pitch);

#endif // STRUCTURES_CHANNEL_HPP
