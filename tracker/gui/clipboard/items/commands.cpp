#include <sstream>

#include "../../../utils/math.hpp"
#include "commands.hpp"

ClipboardCommands::ClipboardCommands(const std::string &nm, const PatternCommands &commands)
    : ClipboardItem(ClipboardCategory::Commands, nm),
      pattern_commands(commands) {
    generate_hash();
}

void ClipboardCommands::generate_hash() {
    size_t seed = 0;
    combine_hash(seed, pattern_commands.size());
    for (const auto &vector : pattern_commands) {
        combine_hash(seed, vector.size());
        for (const auto &[command, value] : vector) {
            const std::string command_value = command + value;
            combine_hash(seed, command_value);
        }
    }

    std::stringstream stream;
    stream << std::hex << seed;
    hash = stream.str();
}
