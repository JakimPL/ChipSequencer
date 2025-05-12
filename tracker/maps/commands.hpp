#ifndef MAPS_COMMANDS_HPP
#define MAPS_COMMANDS_HPP

#include <map>

#include "../constants.hpp"
#include "../sizes.hpp"
#include "../structures/commands/sequence.hpp"

const std::map<uint8_t, uint8_t> command_sizes = {
    {INSTRUCTION_EMPTY, SIZE_COMMAND},
    {INSTRUCTION_PORTAMENTO_UP, SIZE_COMMAND_PORTAMENTO_UP},
    {INSTRUCTION_PORTAMENTO_DOWN, SIZE_COMMAND_PORTAMENTO_DOWN},
    {INSTRUCTION_SET_MASTER_GAINER, SIZE_COMMAND_SET_MASTER_GAINER},
    {INSTRUCTION_SET_BPM, SIZE_COMMAND_SET_BPM},
    {INSTRUCTION_SET_DIVISION, SIZE_COMMAND_SET_DIVISION},
    {INSTRUCTION_CHANGE_BYTE_VALUE, SIZE_COMMAND_CHANGE_BYTE_VALUE},
    {INSTRUCTION_CHANGE_WORD_VALUE, SIZE_COMMAND_CHANGE_WORD_VALUE},
    {INSTRUCTION_CHANGE_DWORD_VALUE, SIZE_COMMAND_CHANGE_DWORD_VALUE},
    {INSTRUCTION_CHANGE_FLOAT_VALUE, SIZE_COMMAND_CHANGE_FLOAT_VALUE}
};

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
    {'M', Instruction::ChangeFloatValue}
};

#endif // MAPS_COMMANDS_HPP
