#include "../../../general.hpp"
#include "../../names.hpp"
#include "add.hpp"

AddItemAction::AddItemAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    Target t,
    AddFunction add,
    RemoveFunction remove
)
    : Action(nm, own, k),
      type(t),
      first_redo(true),
      add_function(add),
      remove_function(remove) {
}

void AddItemAction::redo() {
    if (first_redo) {
        first_redo = false;
        return;
    }

    if (add_function) {
        add_function();
    }
}

void AddItemAction::undo() {
    if (remove_function) {
        size_t index = 0;

        switch (type) {
        case Target::ENVELOPE:
            index = envelopes.size() - 1;
            break;
        case Target::SEQUENCE:
            index = sequences.size() - 1;
            break;
        case Target::ORDER:
            index = orders.size() - 1;
            break;
        case Target::WAVETABLE:
            index = wavetables.size() - 1;
            break;
        case Target::OSCILLATOR:
            index = oscillators.size() - 1;
            break;
        case Target::CHANNEL:
            index = channels.size() - 1;
            break;
        case Target::DSP:
            index = dsps.size() - 1;
            break;
        case Target::COMMANDS_SEQUENCE:
            index = commands_sequences.size() - 1;
            break;
        case Target::COMMANDS_CHANNEL:
            index = commands_channels.size() - 1;
            break;
        case Target::DIRECT_OUTPUT:
        case Target::DIRECT_DSP:
        case Target::SPLITTER_OUTPUT:
        case Target::SPLITTER_DSP:
        case Target::SPECIAL:
        case Target::COUNT:
        default:
            return;
        }

        remove_function(index);
    }
}

std::string AddItemAction::get_name() const {
    const std::string type_name = target_names.at(type);
    return "Add " + type_name;
}
