#ifndef STRUCTURES_COMMANDS_COMMAND_HPP
#define STRUCTURES_COMMANDS_COMMAND_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>

#include "../../constants.hpp"
#include "../../sizes.hpp"
#include "../../song/links/target.hpp"

enum class Instruction : uint8_t {
    Empty = INSTRUCTION_EMPTY,
    PortamentoUp = INSTRUCTION_PORTAMENTO_UP,
    PortamentoDown = INSTRUCTION_PORTAMENTO_DOWN,
    SetMasterGainer = INSTRUCTION_SET_MASTER_GAINER,
    SetBPM = INSTRUCTION_SET_BPM,
    SetDivision = INSTRUCTION_SET_DIVISION,
    ChangeFloatValue = INSTRUCTION_CHANGE_FLOAT_VALUE,
    ChangeByteValue = INSTRUCTION_CHANGE_BYTE_VALUE,
    ChangeWordValue = INSTRUCTION_CHANGE_WORD_VALUE,
    ChangeDwordValue = INSTRUCTION_CHANGE_DWORD_VALUE,
    AddFloatValue = INSTRUCTION_ADD_FLOAT_VALUE,
    AddByteValue = INSTRUCTION_ADD_BYTE_VALUE,
    AddWordValue = INSTRUCTION_ADD_WORD_VALUE,
    AddDwordValue = INSTRUCTION_ADD_DWORD_VALUE,
    Count = INSTRUCTION_COUNT,
};

struct Command {
    uint8_t instruction = INSTRUCTION_EMPTY;
    uint8_t duration;
    uint8_t pad[10] = {0, 0, 0, 0, 0, 0, static_cast<uint8_t>(Target::COUNT), 0, 0, 0};
} __attribute__((packed));

struct CommandPortamentoUp {
    uint8_t instruction = INSTRUCTION_PORTAMENTO_UP;
    uint8_t duration;
    uint16_t value;
    uint8_t channel;
    uint8_t pad[7];
} __attribute__((packed));

struct CommandPortamentoDown {
    uint8_t instruction = INSTRUCTION_PORTAMENTO_DOWN;
    uint8_t duration;
    uint16_t value;
    uint8_t channel;
    uint8_t pad[7];
} __attribute__((packed));

struct CommandSetMasterGainer {
    uint8_t instruction = INSTRUCTION_SET_MASTER_GAINER;
    uint8_t duration;
    uint16_t gain;
    uint8_t pad[8];
} __attribute__((packed));

struct CommandSetBPM {
    uint8_t instruction = INSTRUCTION_SET_BPM;
    uint8_t duration;
    uint16_t bpm;
    uint8_t pad[8];
} __attribute__((packed));

struct CommandSetDivision {
    uint8_t instruction = INSTRUCTION_SET_DIVISION;
    uint8_t duration;
    uint8_t division;
    uint8_t pad[9];
} __attribute__((packed));

struct CommandChangeValue {
    uint8_t instruction = INSTRUCTION_EMPTY;
    uint8_t duration;
    uint16_t pointer;
    uint8_t pad[4];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

struct CommandChangeFloatValue {
    uint8_t instruction = INSTRUCTION_CHANGE_FLOAT_VALUE;
    uint8_t duration;
    uint16_t pointer;
    _Float32 value;
    uint8_t pad[0];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

struct CommandChangeByteValue {
    uint8_t instruction = INSTRUCTION_CHANGE_BYTE_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint8_t value;
    uint8_t pad[3];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

struct CommandChangeWordValue {
    uint8_t instruction = INSTRUCTION_CHANGE_WORD_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint16_t value;
    uint8_t pad[2];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

struct CommandChangeDwordValue {
    uint8_t instruction = INSTRUCTION_CHANGE_DWORD_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint32_t value;
    uint8_t pad[0];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

struct CommandAddFloatValue {
    uint8_t instruction = INSTRUCTION_ADD_FLOAT_VALUE;
    uint8_t duration;
    uint16_t pointer;
    _Float32 value;
    uint8_t pad[0];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

struct CommandAddByteValue {
    uint8_t instruction = INSTRUCTION_ADD_BYTE_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint8_t value;
    uint8_t pad[3];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

struct CommandAddWordValue {
    uint8_t instruction = INSTRUCTION_ADD_WORD_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint16_t value;
    uint8_t pad[2];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

struct CommandAddDwordValue {
    uint8_t instruction = INSTRUCTION_ADD_DWORD_VALUE;
    uint8_t duration;
    uint16_t pointer;
    uint32_t value;
    uint8_t pad[0];
    uint8_t target = static_cast<uint8_t>(Target::COUNT);
    uint8_t index = -1;
    uint16_t offset = 0;
} __attribute__((packed));

typedef std::array<Command, MAX_COMMANDS_SEQUENCES> CommandsArray;

bool is_instruction_linkable(const Instruction instruction);
bool is_instruction_linkable(const uint8_t instruction);
bool is_instruction_float(const uint8_t instruction);
bool is_instruction_float(const Instruction instruction);
bool is_instruction_add(const uint8_t instruction);
bool is_instruction_add(const Instruction instruction);
bool is_instruction_change(const uint8_t instruction);
bool is_instruction_change(const Instruction instruction);

constexpr size_t COMMAND_TYPE = offsetof(Command, instruction);
constexpr size_t COMMAND_DURATION = offsetof(Command, duration);
constexpr size_t COMMAND_PORTAMENTO_UP_CHANNEL = offsetof(CommandPortamentoUp, channel);
constexpr size_t COMMAND_PORTAMENTO_UP_VALUE = offsetof(CommandPortamentoUp, value);
constexpr size_t COMMAND_PORTAMENTO_DOWN_CHANNEL = offsetof(CommandPortamentoDown, channel);
constexpr size_t COMMAND_PORTAMENTO_DOWN_VALUE = offsetof(CommandPortamentoDown, value);
constexpr size_t COMMAND_SET_MASTER_GAINER_GAIN = offsetof(CommandSetMasterGainer, gain);
constexpr size_t COMMAND_SET_BPM_BPM = offsetof(CommandSetBPM, bpm);
constexpr size_t COMMAND_SET_DIVISION_DIVISION = offsetof(CommandSetDivision, division);
constexpr size_t COMMAND_CHANGE_FLOAT_VALUE_POINTER = offsetof(CommandChangeFloatValue, pointer);
constexpr size_t COMMAND_CHANGE_FLOAT_VALUE_VALUE = offsetof(CommandChangeFloatValue, value);
constexpr size_t COMMAND_CHANGE_BYTE_VALUE_POINTER = offsetof(CommandChangeByteValue, pointer);
constexpr size_t COMMAND_CHANGE_BYTE_VALUE_VALUE = offsetof(CommandChangeByteValue, value);
constexpr size_t COMMAND_CHANGE_WORD_VALUE_POINTER = offsetof(CommandChangeWordValue, pointer);
constexpr size_t COMMAND_CHANGE_WORD_VALUE_VALUE = offsetof(CommandChangeWordValue, value);
constexpr size_t COMMAND_CHANGE_DWORD_VALUE_POINTER = offsetof(CommandChangeDwordValue, pointer);
constexpr size_t COMMAND_CHANGE_DWORD_VALUE_VALUE = offsetof(CommandChangeDwordValue, value);
constexpr size_t COMMAND_ADD_FLOAT_VALUE_POINTER = offsetof(CommandAddFloatValue, pointer);
constexpr size_t COMMAND_ADD_FLOAT_VALUE_VALUE = offsetof(CommandAddFloatValue, value);
constexpr size_t COMMAND_ADD_BYTE_VALUE_POINTER = offsetof(CommandAddByteValue, pointer);
constexpr size_t COMMAND_ADD_BYTE_VALUE_VALUE = offsetof(CommandAddByteValue, value);
constexpr size_t COMMAND_ADD_WORD_VALUE_POINTER = offsetof(CommandAddWordValue, pointer);
constexpr size_t COMMAND_ADD_WORD_VALUE_VALUE = offsetof(CommandAddWordValue, value);
constexpr size_t COMMAND_ADD_DWORD_VALUE_POINTER = offsetof(CommandAddDwordValue, pointer);
constexpr size_t COMMAND_ADD_DWORD_VALUE_VALUE = offsetof(CommandAddDwordValue, value);

static_assert(sizeof(Command) == SIZE_COMMAND_EMPTY + sizeof(Command::pad), "Command must be of 2 bytes.");
static_assert(sizeof(CommandPortamentoUp) == SIZE_COMMAND_PORTAMENTO_UP + sizeof(CommandPortamentoUp::pad), "CommandPortamentoUp must be of 5 bytes.");
static_assert(sizeof(CommandPortamentoDown) == SIZE_COMMAND_PORTAMENTO_DOWN + sizeof(CommandPortamentoDown::pad), "CommandPortamentoDown must be of 5 bytes.");
static_assert(sizeof(CommandSetMasterGainer) == SIZE_COMMAND_SET_MASTER_GAINER + sizeof(CommandSetMasterGainer::pad), "CommandSetMasterGainer must be of 4 bytes.");
static_assert(sizeof(CommandSetBPM) == SIZE_COMMAND_SET_BPM + sizeof(CommandSetBPM::pad), "CommandSetBPM must be of 4 bytes.");
static_assert(sizeof(CommandSetDivision) == SIZE_COMMAND_SET_DIVISION + sizeof(CommandSetDivision::pad), "CommandSetDivision must be of 3 bytes.");
static_assert(sizeof(CommandChangeFloatValue) == SIZE_COMMAND_CHANGE_FLOAT_VALUE + sizeof(CommandChangeFloatValue::pad) + sizeof(CommandChangeFloatValue::target) + sizeof(CommandChangeFloatValue::index) + sizeof(CommandChangeFloatValue::offset), "CommandChangeFloatValue must be of 8 bytes.");
static_assert(sizeof(CommandChangeByteValue) == SIZE_COMMAND_CHANGE_BYTE_VALUE + sizeof(CommandChangeByteValue::pad) + sizeof(CommandChangeByteValue::target) + sizeof(CommandChangeByteValue::index) + sizeof(CommandChangeByteValue::offset), "CommandChangeByteValue must be of 5 bytes.");
static_assert(sizeof(CommandChangeWordValue) == SIZE_COMMAND_CHANGE_WORD_VALUE + sizeof(CommandChangeWordValue::pad) + sizeof(CommandChangeWordValue::target) + sizeof(CommandChangeWordValue::index) + sizeof(CommandChangeWordValue::offset), "CommandChangeWordValue must be of 6 bytes.");
static_assert(sizeof(CommandChangeDwordValue) == SIZE_COMMAND_CHANGE_DWORD_VALUE + sizeof(CommandChangeDwordValue::pad) + sizeof(CommandChangeDwordValue::target) + sizeof(CommandChangeDwordValue::index) + sizeof(CommandChangeDwordValue::offset), "CommandChangeDwordValue must be of 8 bytes.");
static_assert(sizeof(CommandAddFloatValue) == SIZE_COMMAND_ADD_FLOAT_VALUE + sizeof(CommandAddFloatValue::pad) + sizeof(CommandAddFloatValue::target) + sizeof(CommandAddFloatValue::index) + sizeof(CommandAddFloatValue::offset), "CommandAddFloatValue must be of 8 bytes.");
static_assert(sizeof(CommandAddByteValue) == SIZE_COMMAND_ADD_BYTE_VALUE + sizeof(CommandAddByteValue::pad) + sizeof(CommandAddByteValue::target) + sizeof(CommandAddByteValue::index) + sizeof(CommandAddByteValue::offset), "CommandAddByteValue must be of 5 bytes.");
static_assert(sizeof(CommandAddWordValue) == SIZE_COMMAND_ADD_WORD_VALUE + sizeof(CommandAddWordValue::pad) + sizeof(CommandAddWordValue::target) + sizeof(CommandAddWordValue::index) + sizeof(CommandAddWordValue::offset), "CommandAddWordValue must be of 6 bytes.");
static_assert(sizeof(CommandAddDwordValue) == SIZE_COMMAND_ADD_DWORD_VALUE + sizeof(CommandAddDwordValue::pad) + sizeof(CommandAddDwordValue::target) + sizeof(CommandAddDwordValue::index) + sizeof(CommandAddDwordValue::offset), "CommandAddDwordValue must be of 8 bytes.");

static_assert(sizeof(Command) == sizeof(CommandPortamentoUp), "Command and CommandPortamentoUp must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandPortamentoDown), "Command and CommandPortamentoDown must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandSetMasterGainer), "Command and CommandSetMasterGainer must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandSetBPM), "Command and CommandSetBPM must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandSetDivision), "Command and CommandSetDivision must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandChangeFloatValue), "Command and CommandChangeFloatValue must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandChangeByteValue), "Command and CommandChangeByteValue must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandChangeWordValue), "Command and CommandChangeWordValue must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandChangeDwordValue), "Command and CommandChangeDwordValue must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandAddFloatValue), "Command and CommandAddFloatValue must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandAddByteValue), "Command and CommandAddByteValue must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandAddWordValue), "Command and CommandAddWordValue must be of the same size.");
static_assert(sizeof(Command) == sizeof(CommandAddDwordValue), "Command and CommandAddDwordValue must be of the same size.");

#endif // STRUCTURES_COMMANDS_COMMAND_HPP
