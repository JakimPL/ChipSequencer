#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "item.hpp"

using CommandValue = std::pair<std::string, std::string>;
using PatternCommands = std::vector<std::vector<CommandValue>>;

struct ClipboardCommands : public ClipboardItem {
    explicit ClipboardCommands(PatternCommands commands);
    PatternCommands pattern_commands;

    void generate_hash() override;
    void generate_name() override;
};
