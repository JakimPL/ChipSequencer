#include "commands.hpp"

CommandsPattern::CommandsPattern()
    : input_handler(
          commands,
          current_row,
          std::vector<ImGuiKey>(std::begin(commands_keys), std::end(commands_keys))
      ) {
    input_handler.set_limit(MAX_COMMAND_COMMAND_SIZE);
}

void CommandsPattern::handle_input(const int min_row, const int max_row) {
    const bool valid = current_row >= 0 && current_row < commands.size();
    if (!valid) {
        return;
    }

    input_handler.handle_input();
}
