#include "command.hpp"

bool is_command_linkable(const Instruction instruction) {
    return instruction == Instruction::ChangeFloatValue ||
           instruction == Instruction::ChangeByteValue ||
           instruction == Instruction::ChangeWordValue ||
           instruction == Instruction::ChangeDwordValue ||
           instruction == Instruction::AddFloatValue ||
           instruction == Instruction::AddByteValue ||
           instruction == Instruction::AddWordValue ||
           instruction == Instruction::AddDwordValue;
}

bool is_command_linkable(const uint8_t instruction) {
    return is_command_linkable(static_cast<Instruction>(instruction));
}
