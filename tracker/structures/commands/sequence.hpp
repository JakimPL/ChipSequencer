#ifndef STRUCTURES_COMMANDS_SEQUENCE_HPP
#define STRUCTURES_COMMANDS_SEQUENCE_HPP

#include <cstdint>
#include <fstream>
#include <vector>

#include "command.hpp"

struct CommandsSequence {
    uint16_t size;
    uint8_t length;
    CommandsArray commands;

    void from_command_vector(const std::vector<Command> &command_vector);
    void serialize(std::ofstream &file) const;
    static CommandsSequence *deserialize(std::ifstream &file);
};

typedef std::vector<CommandsSequence *> CommandsSequences;

constexpr size_t COMMANDS_SEQUENCE_SIZE = offsetof(CommandsSequence, size);
constexpr size_t COMMANDS_SEQUENCE_LENGTH = offsetof(CommandsSequence, length);
constexpr size_t COMMANDS_SEQUENCE_DATA = offsetof(CommandsSequence, commands);

#endif // STRUCTURES_COMMANDS_SEQUENCE_HPP
