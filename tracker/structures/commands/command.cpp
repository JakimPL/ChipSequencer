#include "command.hpp"

bool is_instruction_linkable(const Instruction instruction) {
    return instruction == Instruction::ChangeFloatValue ||
           instruction == Instruction::ChangeByteValue ||
           instruction == Instruction::ChangeWordValue ||
           instruction == Instruction::ChangeDwordValue ||
           instruction == Instruction::AddFloatValue ||
           instruction == Instruction::AddByteValue ||
           instruction == Instruction::AddWordValue ||
           instruction == Instruction::AddDwordValue;
}

bool is_instruction_linkable(const uint8_t instruction) {
    return is_instruction_linkable(static_cast<Instruction>(instruction));
}

bool is_instruction_float(const Instruction instruction) {
    return instruction == Instruction::ChangeFloatValue ||
           instruction == Instruction::AddFloatValue;
}

bool is_instruction_float(const uint8_t instruction) {
    return is_instruction_float(static_cast<Instruction>(instruction));
}

bool is_instruction_add(const Instruction instruction) {
    return instruction == Instruction::AddFloatValue ||
           instruction == Instruction::AddByteValue ||
           instruction == Instruction::AddWordValue ||
           instruction == Instruction::AddDwordValue;
}

bool is_instruction_add(const uint8_t instruction) {
    return is_instruction_add(static_cast<Instruction>(instruction));
}

bool is_instruction_change(const Instruction instruction) {
    return instruction == Instruction::ChangeFloatValue ||
           instruction == Instruction::ChangeByteValue ||
           instruction == Instruction::ChangeWordValue ||
           instruction == Instruction::ChangeDwordValue;
}

bool is_instruction_change(const uint8_t instruction) {
    return is_instruction_change(static_cast<Instruction>(instruction));
}
