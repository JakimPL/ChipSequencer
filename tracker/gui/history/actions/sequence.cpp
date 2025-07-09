#include <sstream>
#include <utility>

#include "../../panels/orders.hpp"
#include "../../panels/sequences.hpp"
#include "../../panels/commands/sequences.hpp"
#include "../../patterns/commands.hpp"
#include "sequence.hpp"

template <typename T, typename Panel>
ChangeSequenceAction<T, Panel>::ChangeSequenceAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    SequenceChange<T> seq_ch
)
    : Action(nm, own, k), sequence_change(std::move(seq_ch)) {
}

template <typename T, typename Panel>
void ChangeSequenceAction<T, Panel>::redo() {
    Panel *panel = dynamic_cast<Panel *>(owner);
    if (panel != nullptr) {
        panel->set_sequence(sequence_change.new_sequence);
    }
}

template <typename T, typename Panel>
void ChangeSequenceAction<T, Panel>::undo() {
    Panel *panel = dynamic_cast<Panel *>(owner);
    if (panel != nullptr) {
        panel->set_sequence(sequence_change.old_sequence);
    }
}

template <typename T, typename Panel>
bool ChangeSequenceAction<T, Panel>::can_merge(const Action *other) const {
    if (!Action::can_merge(other)) {
        return false;
    }
    return dynamic_cast<const ChangeSequenceAction *>(other) != nullptr;
}

template <typename T, typename Panel>
void ChangeSequenceAction<T, Panel>::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeSequenceAction *>(other);
    if (other_change != nullptr) {
        sequence_change.new_sequence = other_change->sequence_change.new_sequence;
    }
}

template <typename T, typename Panel>
std::string ChangeSequenceAction<T, Panel>::get_name() const {
    std::ostringstream stream;
    stream << "Change " << name;
    return stream.str();
}

template class ChangeSequenceAction<uint8_t, GUISequencesPanel>;
template class ChangeSequenceAction<CommandValue, GUICommandsSequencesPanel>;
template class ChangeSequenceAction<int, GUIOrdersPanel>;
