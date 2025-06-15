#pragma once

#include <array>
#include <cstdint>
#include <map>

#include "../constants.hpp"
#include "../structures/sizes.hpp"
#include "../structures/commands/sequence.hpp"

template <typename... T>
constexpr std::array<uint8_t, sizeof...(T)> make_commands_sizes(T... args) {
    static_assert(sizeof...(T) == static_cast<size_t>(Instruction::Count), "Number of command sizes must match Instruction::Count");
    return {static_cast<uint8_t>(args)...};
}

constexpr auto commands_sizes = make_commands_sizes(
    SIZE_COMMAND_EMPTY,
    SIZE_COMMAND_PORTAMENTO_UP,
    SIZE_COMMAND_PORTAMENTO_DOWN,
    SIZE_COMMAND_SET_MASTER_GAINER,
    SIZE_COMMAND_SET_BPM,
    SIZE_COMMAND_SET_DIVISION,
    SIZE_COMMAND_CHANGE_BYTE_VALUE,
    SIZE_COMMAND_CHANGE_WORD_VALUE,
    SIZE_COMMAND_CHANGE_DWORD_VALUE,
    SIZE_COMMAND_CHANGE_FLOAT_VALUE,
    SIZE_COMMAND_ADD_BYTE_VALUE,
    SIZE_COMMAND_ADD_WORD_VALUE,
    SIZE_COMMAND_ADD_DWORD_VALUE,
    SIZE_COMMAND_ADD_FLOAT_VALUE
);

const std::map<char, Instruction> command_characters = {
    {' ', Instruction::Empty},
    {'U', Instruction::PortamentoUp},
    {'D', Instruction::PortamentoDown},
    {'G', Instruction::SetMasterGainer},
    {'B', Instruction::SetBPM},
    {'S', Instruction::SetDivision},
    {'M', Instruction::ChangeByteValue},
    {'M', Instruction::ChangeWordValue},
    {'M', Instruction::ChangeDwordValue},
    {'M', Instruction::ChangeFloatValue},
    {'A', Instruction::AddByteValue},
    {'A', Instruction::AddWordValue},
    {'A', Instruction::AddDwordValue},
    {'A', Instruction::AddFloatValue},
};

const std::map<Instruction, char> command_letters = {
    {Instruction::Empty, ' '},
    {Instruction::PortamentoUp, 'U'},
    {Instruction::PortamentoDown, 'D'},
    {Instruction::SetMasterGainer, 'G'},
    {Instruction::SetBPM, 'B'},
    {Instruction::SetDivision, 'S'},
    {Instruction::ChangeByteValue, 'M'},
    {Instruction::ChangeWordValue, 'M'},
    {Instruction::ChangeDwordValue, 'M'},
    {Instruction::ChangeFloatValue, 'M'},
    {Instruction::AddByteValue, 'A'},
    {Instruction::AddWordValue, 'A'},
    {Instruction::AddDwordValue, 'A'},
    {Instruction::AddFloatValue, 'A'},
};
