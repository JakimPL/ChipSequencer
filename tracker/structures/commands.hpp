#ifndef STRUCTURES_COMMANDS_HPP
#define STRUCTURES_COMMANDS_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

typedef std::array<uint8_t, UINT8_MAX> CommandsArray;

struct CommandsSequence {
    uint8_t size;
    CommandsArray commands;
};

typedef std::vector<CommandsSequence *> Commands;

constexpr size_t COMMANDS_SIZE = offsetof(CommandsSequence, size);
constexpr size_t COMMANDS_DATA = offsetof(CommandsSequence, commands);

#endif // STRUCTURES_COMMANDS_HPP
