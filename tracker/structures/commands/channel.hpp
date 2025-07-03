#pragma once

#include <cstdint>
#include <vector>

#include "../../structures/sizes.hpp"

struct CommandsChannel {
    uint8_t flag;
    uint8_t order_index;
} __attribute__((packed));

using CommandsChannels = std::vector<CommandsChannel *>;

constexpr size_t COMMANDS_CHANNEL_FLAG = offsetof(CommandsChannel, flag);
constexpr size_t COMMANDS_CHANNEL_ORDER_INDEX = offsetof(CommandsChannel, order_index);

static_assert(sizeof(CommandsChannel) == SIZE_COMMANDS_CHANNEL, "CommandsChannel must be of 2 bytes.");
