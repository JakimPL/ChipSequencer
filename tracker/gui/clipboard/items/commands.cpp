#include <sstream>

#include "../../../utils/math.hpp"
#include "commands.hpp"

ClipboardCommands::ClipboardCommands(const PatternCommands &commands)
    : ClipboardItem(ClipboardCategory::Commands), pattern_commands(commands) {
    generate_hash();
    generate_name();
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

void ClipboardCommands::generate_name() {
    if (pattern_commands.empty()) {
        name = "Empty pattern commands selection";
        return;
    }

    int count = 0;
    std::stringstream stream;
    stream << "Commands (" << pattern_commands.size() << " channels): ";
    for (size_t channel = 0; channel < pattern_commands.size(); ++channel) {
        const auto &commands = pattern_commands[channel];
        for (int i = 0; i < commands.size(); ++i) {
            const auto &[command, value] = commands[i];
            const std::string command_value = command.empty() ? "E" : command + " " + value;

            if (count > MAX_CLIPBOARD_ENUMERATION) {
                stream << ", ...";
                name = stream.str();
                return;
            }

            ++count;
            stream << command_value;
            if (i < commands.size() - 1 || &commands < &pattern_commands.back()) {
                stream << ", ";
            }
        }
    }

    name = stream.str();
}