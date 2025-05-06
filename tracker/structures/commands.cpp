#include "commands.hpp"

void CommandsSequence::from_command_vector(const std::vector<Command> &command_vector) {
    size = 0;
    length = 0;
    for (const Command &command : command_vector) {
        commands[length] = command;
        switch (static_cast<Instruction>(command.instruction)) {
        case Instruction::PortamentoUp: {
            size += SIZE_COMMAND_PORTAMENTO_UP;
            break;
        }
        case Instruction::PortamentoDown: {
            size += SIZE_COMMAND_PORTAMENTO_DOWN;
            break;
        }
        case Instruction::SetMasterGainer: {
            size += SIZE_COMMAND_SET_MASTER_GAINER;
            break;
        }
        case Instruction::SetBPM: {
            size += SIZE_COMMAND_SET_BPM;
            break;
        }
        case Instruction::SetDivision: {
            size += SIZE_COMMAND_SET_DIVISION;
            break;
        }
        case Instruction::ChangeByteValue: {
            size += SIZE_COMMAND_CHANGE_BYTE_VALUE;
            break;
        }
        case Instruction::ChangeWordValue: {
            size += SIZE_COMMAND_CHANGE_WORD_VALUE;
            break;
        }
        case Instruction::ChangeDwordValue: {
            size += SIZE_COMMAND_CHANGE_DWORD_VALUE;
            break;
        }
        case Instruction::ChangeFloatValue: {
            size += SIZE_COMMAND_CHANGE_FLOAT_VALUE;
            break;
        }
        case Instruction::Empty: {
            size += SIZE_COMMAND;
            break;
        }
        default:
            throw std::runtime_error("Invalid command type");
        }

        length++;
    }
}
