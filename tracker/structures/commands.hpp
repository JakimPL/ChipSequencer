#ifndef STRUCTURES_COMMANDS_HPP
#define STRUCTURES_COMMANDS_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <variant>
#include <vector>

#include "../constants.hpp"
#include "../sizes.hpp"

enum class Instruction : uint8_t {
    Empty = INSTRUCTION_EMPTY,
    PortamentoUp = INSTRUCTION_PORTAMENTO_UP,
    PortamentoDown = INSTRUCTION_PORTAMENTO_DOWN,
    SetMasterGainer = INSTRUCTION_SET_MASTER_GAINER,
    SetBPM = INSTRUCTION_SET_BPM,
    SetDivision = INSTRUCTION_SET_DIVISION,
    ChangeByteValue = INSTRUCTION_CHANGE_BYTE_VALUE,
    ChangeWordValue = INSTRUCTION_CHANGE_WORD_VALUE,
    ChangeDwordValue = INSTRUCTION_CHANGE_DWORD_VALUE,
    ChangeFloatValue = INSTRUCTION_CHANGE_FLOAT_VALUE,
};

struct Command {
    uint8_t instruction = INSTRUCTION_EMPTY;
    uint8_t duration;
    uint8_t pad[6];
};

struct CommandPortamentoUp {
    uint8_t instruction = INSTRUCTION_PORTAMENTO_UP;
    uint8_t duration;
    uint16_t value;
    uint8_t channel;
    uint8_t pad[3];
};

struct CommandPortamentoDown {
    uint8_t instruction = INSTRUCTION_PORTAMENTO_DOWN;
    uint8_t duration;
    uint16_t value;
    uint8_t channel;
    uint8_t pad[3];
};

struct CommandSetMasterGainer {
    uint8_t instruction = INSTRUCTION_SET_MASTER_GAINER;
    uint8_t duration;
    uint16_t gain;
    uint8_t pad[4];
};

struct CommandSetBPM {
    uint8_t instruction = INSTRUCTION_SET_BPM;
    uint8_t duration;
    uint16_t bpm;
    uint8_t pad[4];
};

struct CommandSetDivision {
    uint8_t instruction = INSTRUCTION_SET_DIVISION;
    uint8_t duration;
    uint8_t division;
    uint8_t pad[5];
};

struct CommandChangeByteValue {
    uint8_t instruction = INSTRUCTION_CHANGE_BYTE_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint8_t value;
    uint8_t pad[3];
};

struct CommandChangeWordValue {
    uint8_t instruction = INSTRUCTION_CHANGE_WORD_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint16_t value;
    uint8_t pad[2];
};

struct CommandChangeDwordValue {
    uint8_t instruction = INSTRUCTION_CHANGE_DWORD_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint32_t value;
};

struct CommandChangeFloatValue {
    uint8_t instruction = INSTRUCTION_CHANGE_FLOAT_VALUE;
    uint8_t duration;
    uint16_t pointer;
    _Float32 value;
};

typedef std::array<Command, UINT8_MAX> CommandsArray;

struct CommandsSequence {
    uint16_t size;
    uint8_t length;
    CommandsArray commands;

    void from_command_vector(const std::vector<Command> &command_vector);
};

typedef std::vector<CommandsSequence *> CommandsSequences;

constexpr size_t COMMAND_TYPE = offsetof(Command, instruction);
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

static_assert(sizeof(Command) == SIZE_COMMAND + sizeof(Command::pad), "Command size mismatch");
static_assert(sizeof(CommandPortamentoUp) == SIZE_COMMAND_PORTAMENTO_UP + sizeof(CommandPortamentoUp::pad), "CommandPortamentoUp size mismatch");
static_assert(sizeof(CommandPortamentoDown) == SIZE_COMMAND_PORTAMENTO_DOWN + sizeof(CommandPortamentoDown::pad), "CommandPortamentoDown size mismatch");
static_assert(sizeof(CommandSetMasterGainer) == SIZE_COMMAND_SET_MASTER_GAINER + sizeof(CommandSetMasterGainer::pad), "CommandSetMasterGainer size mismatch");
static_assert(sizeof(CommandSetBPM) == SIZE_COMMAND_SET_BPM + sizeof(CommandSetBPM::pad), "CommandSetBPM size mismatch");
static_assert(sizeof(CommandSetDivision) == SIZE_COMMAND_SET_DIVISION + sizeof(CommandSetDivision::pad), "CommandSetDivision size mismatch");
static_assert(sizeof(CommandChangeByteValue) == SIZE_COMMAND_CHANGE_BYTE_VALUE + sizeof(CommandChangeByteValue::pad), "CommandChangeByteValue size mismatch");
static_assert(sizeof(CommandChangeWordValue) == SIZE_COMMAND_CHANGE_WORD_VALUE + sizeof(CommandChangeWordValue::pad), "CommandChangeWordValue size mismatch");
static_assert(sizeof(CommandChangeDwordValue) == SIZE_COMMAND_CHANGE_DWORD_VALUE, "CommandChangeDwordValue size mismatch");
static_assert(sizeof(CommandChangeFloatValue) == SIZE_COMMAND_CHANGE_FLOAT_VALUE, "CommandChangeFloatValue size mismatch");

#endif // STRUCTURES_COMMANDS_HPP
