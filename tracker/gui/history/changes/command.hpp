#pragma once

#include "../../patterns/commands.hpp"
#include "../../patterns/selection.hpp"

struct CommandChange {
    PatternRow pattern_row;
    CommandValue old_command;
    CommandValue new_command;
};
