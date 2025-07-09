#include <sstream>
#include <utility>

#include "../../panels/sequences.hpp"
#include "sequence.hpp"

ChangeSequenceAction::ChangeSequenceAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    SequenceChange seq_ch
)
    : Action(nm, own, k), sequence_change(std::move(seq_ch)) {
}

void ChangeSequenceAction::redo() {
    GUISequencesPanel *panel = dynamic_cast<GUISequencesPanel *>(owner);
    if (panel != nullptr) {
        panel->set_sequence(sequence_change.sequence_index, sequence_change.new_sequence);
    }
}

void ChangeSequenceAction::undo() {
    GUISequencesPanel *panel = dynamic_cast<GUISequencesPanel *>(owner);
    if (panel != nullptr) {
        panel->set_sequence(sequence_change.sequence_index, sequence_change.old_sequence);
    }
}

bool ChangeSequenceAction::can_merge(const Action *other) const {
    if (!Action::can_merge(other)) {
        return false;
    }
    return dynamic_cast<const ChangeSequenceAction *>(other) != nullptr;
}

void ChangeSequenceAction::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeSequenceAction *>(other);
    if (other_change != nullptr) {
        sequence_change.new_sequence = other_change->sequence_change.new_sequence;
    }
}

std::string ChangeSequenceAction::get_name() const {
    std::ostringstream stream;
    stream << "Change " << name;
    return stream.str();
}
