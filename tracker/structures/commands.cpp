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
        }
        case Instruction::SetBPM: {
        }
        case Instruction::SetDivision: {
        }
        case Instruction::ChangeByteValue: {
        }
        case Instruction::ChangeWordValue: {
        }
        case Instruction::ChangeDwordValue: {
        }
        case Instruction::ChangeFloatValue: {
        }
        case Instruction::Empty: {
        }
        default:
            throw std::runtime_error("Invalid command type");
        }

        length++;
    }
}
