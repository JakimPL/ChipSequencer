#ifndef GUI_PATTERNS_COMMANDS_HPP
#define GUI_PATTERNS_COMMANDS_HPP

#include <cstdint>
#include <vector>

#include "../../structures/commands.hpp"

struct CommandsPattern {
    uint8_t sequence_index = -1;
    int current_row = -1;
    int steps = 0;
    std::vector<uint8_t> commands = {};
    std::vector<uint8_t> durations = {};
};

#endif // GUI_PATTERNS_COMMANDS_HPP
