#include "../maps/commands.hpp"
#include "../utils/file.hpp"
#include "commands.hpp"

void CommandsSequence::from_command_vector(const std::vector<Command> &command_vector) {
    size = 0;
    length = 0;
    for (const Command &command : command_vector) {
        commands[length] = command;
        size += command_sizes.at(command.instruction);
        length++;
    }
}

void CommandsSequence::serialize(std::ofstream &file) const {
    write_data(file, &size, sizeof(size));
    write_data(file, &length, sizeof(length));
    for (uint8_t i = 0; i < length; i++) {
        const uint8_t command_size = command_sizes.at(commands[i].instruction);
        write_data(file, &commands[i], command_size);
    }
}

CommandsSequence *CommandsSequence::deserialize(std::ifstream &file) {
    CommandsSequence *commands_sequence = new CommandsSequence();
    read_data(file, &commands_sequence->size, sizeof(commands_sequence->size));
    read_data(file, &commands_sequence->length, sizeof(commands_sequence->length));
    for (uint8_t i = 0; i < commands_sequence->length; i++) {
        const std::streampos stream_position = file.tellg();

        Command command;
        uint8_t instruction;
        read_data(file, &instruction, sizeof(instruction));
        const uint8_t command_size = command_sizes.at(instruction);

        file.seekg(stream_position);
        read_data(file, &commands_sequence->commands[i], command_size);
    }

    return commands_sequence;
}
