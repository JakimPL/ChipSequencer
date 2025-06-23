#include <sstream>

#include "wavetable.hpp"

ChangeWavetableAction::ChangeWavetableAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    const WavetableChange &wt_ch
)
    : Action(nm, own, k), wavetable_change(wt_ch) {
}

void ChangeWavetableAction::redo() {
    wavetable_change.wavetable = wavetable_change.new_wave;
    wavetable_change.wavetable_size = wavetable_change.new_wave.size();
}

void ChangeWavetableAction::undo() {
    wavetable_change.wavetable = wavetable_change.old_wave;
    wavetable_change.wavetable_size = wavetable_change.old_wave.size();
}

bool ChangeWavetableAction::can_merge(const Action *other) const {
    if (!Action::can_merge(other)) {
        return false;
    }
    return dynamic_cast<const ChangeWavetableAction *>(other) != nullptr;
}

void ChangeWavetableAction::merge(const Action *other) {
    const auto *other_change = dynamic_cast<const ChangeWavetableAction *>(other);
    if (other_change) {
        wavetable_change.new_wave = other_change->wavetable_change.new_wave;
        wavetable_change.wavetable_size = other_change->wavetable_change.new_wave.size();
    }
}

std::string ChangeWavetableAction::get_name() const {
    std::ostringstream stream;
    stream << "Change " << name;
    return stream.str();
}
