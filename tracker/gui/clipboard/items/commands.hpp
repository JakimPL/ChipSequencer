#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "item.hpp"

typedef std::pair<std::string, std::string> CommandValue;
typedef std::vector<std::vector<CommandValue>> PatternCommands;

struct ClipboardCommands : public ClipboardItem {
    ClipboardCommands(const std::string &nm, const PatternCommands &commands);
    PatternCommands pattern_commands;

    void generate_hash() override;
};
