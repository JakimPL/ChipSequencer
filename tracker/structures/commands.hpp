#ifndef STRUCTURES_COMMANDS_HPP
#define STRUCTURES_COMMANDS_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>

#include "../constants.hpp"

struct Command {
    uint8_t type = COMMAND_EMPTY;
    uint8_t duration;
    uint8_t pad[6];
};

struct CommandPortamentoUp {
    uint8_t type = COMMAND_PORTAMENTO_UP;
    uint8_t duration;
    uint8_t channel;
    uint16_t value;
    uint8_t pad[3];
};

struct CommandPortamentoDown {
    uint8_t type = COMMAND_PORTAMENTO_DOWN;
    uint8_t duration;
    uint8_t channel;
    uint16_t value;
    uint8_t pad[3];
};

struct CommandSetMasterGainer {
    uint8_t type = COMMAND_SET_MASTER_GAINER;
    uint8_t duration;
    uint16_t gain;
    uint8_t pad[4];
};

struct CommandSetBPM {
    uint8_t type = COMMAND_SET_BPM;
    uint8_t duration;
    uint16_t bpm;
    uint8_t pad[4];
};

struct CommandSetDivision {
    uint8_t type = COMMAND_SET_DIVISION;
    uint8_t duration;
    uint8_t division;
    uint8_t pad[5];
};

struct CommandChangeByteValue {
    uint8_t type = COMMAND_CHANGE_BYTE_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint8_t value;
    uint8_t pad[3];
};

struct CommandChangeWordValue {
    uint8_t type = COMMAND_CHANGE_WORD_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint16_t value;
    uint8_t pad[2];
};

struct CommandChangeDwordValue {
    uint8_t type = COMMAND_CHANGE_DWORD_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint32_t value;
};

struct CommandChangeFloatValue {
    uint8_t type = COMMAND_CHANGE_FLOAT_VALUE;
    uint8_t duration;
    uint16_t pointer;
    _Float32 value;
};

using CommandVariant = std::variant<
    CommandPortamentoUp,
    CommandPortamentoDown,
    CommandSetMasterGainer,
    CommandSetBPM,
    CommandSetDivision,
    CommandChangeByteValue,
    CommandChangeWordValue,
    CommandChangeDwordValue,
    CommandChangeFloatValue>;

typedef std::array<CommandVariant, UINT8_MAX> CommandsArray;

struct CommandsSequence {
    uint16_t size;
    uint8_t length;
    CommandsArray commands;
};

typedef std::vector<CommandsSequence *> CommandsSequences;

constexpr size_t COMMAND_TYPE = offsetof(Command, type);
constexpr size_t COMMAND_DURATION = offsetof(Command, duration);
constexpr size_t COMMAND_PORTAMENTO_UP_CHANNEL = offsetof(CommandPortamentoUp, channel);
constexpr size_t COMMAND_PORTAMENTO_UP_VALUE = offsetof(CommandPortamentoUp, value);
constexpr size_t COMMAND_PORTAMENTO_DOWN_CHANNEL = offsetof(CommandPortamentoDown, channel);
constexpr size_t COMMAND_PORTAMENTO_DOWN_VALUE = offsetof(CommandPortamentoDown, value);
constexpr size_t COMMAND_SET_MASTER_GAINER_GAIN = offsetof(CommandSetMasterGainer, gain);
constexpr size_t COMMAND_SET_BPM_BPM = offsetof(CommandSetBPM, bpm);
constexpr size_t COMMAND_SET_DIVISION_DIVISION = offsetof(CommandSetDivision, division);
constexpr size_t COMMAND_CHANGE_BYTE_VALUE_POINTER = offsetof(CommandChangeByteValue, pointer);
constexpr size_t COMMAND_CHANGE_BYTE_VALUE_VALUE = offsetof(CommandChangeByteValue, value);
constexpr size_t COMMAND_CHANGE_WORD_VALUE_POINTER = offsetof(CommandChangeWordValue, pointer);
constexpr size_t COMMAND_CHANGE_WORD_VALUE_VALUE = offsetof(CommandChangeWordValue, value);
constexpr size_t COMMAND_CHANGE_DWORD_VALUE_POINTER = offsetof(CommandChangeDwordValue, pointer);
constexpr size_t COMMAND_CHANGE_DWORD_VALUE_VALUE = offsetof(CommandChangeDwordValue, value);
constexpr size_t COMMAND_CHANGE_FLOAT_VALUE_POINTER = offsetof(CommandChangeFloatValue, pointer);
constexpr size_t COMMAND_CHANGE_FLOAT_VALUE_VALUE = offsetof(CommandChangeFloatValue, value);

constexpr size_t COMMANDS_SIZE = offsetof(CommandsSequence, size);
constexpr size_t COMMANDS_DATA = offsetof(CommandsSequence, commands);

#endif // STRUCTURES_COMMANDS_HPP
