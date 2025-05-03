#ifndef GUI_PATTERNS_COMMANDS_HPP
#define GUI_PATTERNS_COMMANDS_HPP

#include <cstdint>
#include <vector>

#include "../../structures/commands.hpp"
#include "../init.hpp"
#include "../input.hpp"

static std::vector<ImGuiKey> commands_keys = {
    ImGuiKey_U,
    ImGuiKey_D,
    ImGuiKey_M,
    ImGuiKey_B,
    ImGuiKey_S,
    ImGuiKey_G,
};

struct CommandsPattern {
    uint8_t sequence_index = -1;
    int current_row = -1;
    int steps = 0;
    std::vector<std::string> commands = {};
    std::vector<uint8_t> durations = {};

    InputHandler input_handler = InputHandler(commands, current_row, commands_keys);

    CommandsPattern();
    void handle_input(const int min_row = 0, const int max_row = -1);
};

#endif // GUI_PATTERNS_COMMANDS_HPP
