#ifndef STRUCTURES_COMMANDS_HPP
#define STRUCTURES_COMMANDS_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <variant>
#include <vector>

enum class CommandType : uint8_t {
    Empty,
    PortamentoUp,
    PortamentoDown,
    SetMasterGainer,
    SetBPM,
    SetDivision,
    ChangeByteValue,
    ChangeWordValue,
    ChangeDwordValue,
    ChangeFloatValue,
};

struct Command {
    uint8_t type = static_cast<uint8_t>(CommandType::Empty);
    uint8_t duration;
};

struct CommandPortamentoUp {
    uint8_t type = static_cast<uint8_t>(CommandType::PortamentoUp);
    uint8_t duration;
    uint8_t channel;
    uint16_t value;
};

struct CommandPortamentoDown {
    uint8_t type = static_cast<uint8_t>(CommandType::PortamentoDown);
    uint8_t duration;
    uint8_t channel;
    uint16_t value;
};

struct CommandSetMasterGainer {
    uint8_t type = static_cast<uint8_t>(CommandType::SetMasterGainer);
    uint8_t duration;
    uint16_t gain;
};

struct CommandSetBPM {
    uint8_t type = static_cast<uint8_t>(CommandType::SetBPM);
    uint8_t duration;
    uint16_t bpm;
};

struct CommandSetDivision {
    uint8_t type = static_cast<uint8_t>(CommandType::SetDivision);
    uint8_t duration;
    uint8_t division;
};

struct CommandChangeByteValue {
    uint8_t type = static_cast<uint8_t>(CommandType::ChangeByteValue);
    uint8_t duration;
    uint16_t pointer;
    uint8_t value;
};

struct CommandChangeWordValue {
    uint8_t type = static_cast<uint8_t>(CommandType::ChangeWordValue);
    uint8_t duration;
    uint16_t pointer;
    uint16_t value;
};

struct CommandChangeDwordValue {
    uint8_t type = static_cast<uint8_t>(CommandType::ChangeDwordValue);
    uint8_t duration;
    uint16_t pointer;
    uint32_t value;
};

struct CommandChangeFloatValue {
    uint8_t type = static_cast<uint8_t>(CommandType::ChangeFloatValue);
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
