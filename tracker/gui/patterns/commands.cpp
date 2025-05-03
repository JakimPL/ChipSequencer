#include "commands.hpp"

CommandsPattern::CommandsPattern() {
    input_handler.set_limit(1);
}

void CommandsPattern::handle_input(const int min_row, const int max_row) {
    const bool valid = current_row >= 0 && current_row < commands.size();
    if (!valid) {
        return;
    }

    input_handler.handle_input();
}
