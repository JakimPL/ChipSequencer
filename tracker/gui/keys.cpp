#include <stdexcept>
#include <string>

#include "../structures/channel.hpp"
#include "../structures/dsp.hpp"
#include "keys.hpp"

OutputKey from_input_key(const InputKey &source_key) {
    switch (source_key.first) {
    case ItemType::CHANNEL: {
        return {Target::CHANNEL, source_key.second, CHANNEL_TARGET};
    }
    case ItemType::DSP: {
        return {Target::DSP, source_key.second, DSP_TARGET};
    }
    case ItemType::COMMANDS:
    case ItemType::COUNT:
    default:
        throw std::runtime_error("Invalid source type for routing link: " + std::to_string(static_cast<int>(source_key.first)));
    }
}
