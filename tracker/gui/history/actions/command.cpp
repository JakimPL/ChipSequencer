#include <sstream>
#include <stdexcept>

#include "../../names.hpp"
#include "../../panels/patterns.hpp"
#include "../../panels/commands/sequences.hpp"
#include "command.hpp"

ChangeCommandAction::ChangeCommandAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    const CommandChange &cmd_ch
)
    : Action(nm, own, k), command_change(cmd_ch) {
}

void ChangeCommandAction::redo() {
    GUIElement element = owner->get_element();
    if (element == GUIElement::Patterns) {
        GUIPatternsPanel *panel = static_cast<GUIPatternsPanel *>(owner);
        panel->set_command(command_change.pattern_row, command_change.new_command);
    } else if (element == GUIElement::CommandsSequences) {
        GUICommandsSequencesPanel *panel = static_cast<GUICommandsSequencesPanel *>(owner);
        panel->set_command(command_change.pattern_row, command_change.new_command);
    } else {
        throw std::runtime_error("Invalid GUIElement for ChangeCommandAction");
    }
}

void ChangeCommandAction::undo() {
    GUIElement element = owner->get_element();
    if (element == GUIElement::Patterns) {
        GUIPatternsPanel *panel = static_cast<GUIPatternsPanel *>(owner);
        panel->set_command(command_change.pattern_row, command_change.old_command);
    } else if (element == GUIElement::CommandsSequences) {
        GUICommandsSequencesPanel *panel = static_cast<GUICommandsSequencesPanel *>(owner);
        panel->set_command(command_change.pattern_row, command_change.old_command);
    } else {
        throw std::runtime_error("Invalid GUIElement for ChangeCommandAction");
    }
}

bool ChangeCommandAction::can_merge(const Action *other) const {
    const ChangeCommandAction *change_command_action = dynamic_cast<const ChangeCommandAction *>(other);
    if (change_command_action == nullptr) {
        return false;
    }

    return change_command_action->get_pattern_row() == command_change.pattern_row;
}

void ChangeCommandAction::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeCommandAction *>(other);
    if (other_change) {
        command_change.new_command = other_change->command_change.new_command;
    }
}

std::string ChangeCommandAction::get_name() const {
    std::ostringstream oss;
    oss << "Change " << name
        << " row " << command_change.pattern_row.row
        << " command " << get_command_name(command_change.old_command)
        << " to " << get_command_name(command_change.new_command);

    return oss.str();
}

PatternRow ChangeCommandAction::get_pattern_row() const {
    return command_change.pattern_row;
}
