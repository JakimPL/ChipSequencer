#ifndef STRUCTURES_COMMANDS_CHANNEL_HPP
#define STRUCTURES_COMMANDS_CHANNEL_HPP

#include <cstdint>
#include <vector>

#include "../../sizes.hpp"

struct CommandsChannel {
    uint8_t flag;
    uint8_t order_index;
};

typedef std::vector<CommandsChannel *> CommandsChannels;

constexpr size_t COMMANDS_CHANNEL_FLAG = offsetof(CommandsChannel, flag);
constexpr size_t COMMANDS_CHANNEL_ORDER_INDEX = offsetof(CommandsChannel, order_index);

static_assert(sizeof(CommandsChannel) == SIZE_COMMANDS_CHANNEL, "CommandsChannel must be of 2 bytes.");

#endif // STRUCTURES_COMMANDS_CHANNEL_HPP
