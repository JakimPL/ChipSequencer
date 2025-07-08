#include <sstream>
#include <utility>

#include "../../../utils/math.hpp"
#include "../../names.hpp"
#include "notes.hpp"

ClipboardNotes::ClipboardNotes(PatternNotes notes)
    : ClipboardItem(ClipboardCategory::Notes), pattern_notes(std::move(notes)) {
    ClipboardNotes::generate_hash();
    ClipboardNotes::generate_name();
}

void ClipboardNotes::generate_hash() {
    size_t seed = 0;
    combine_hash(seed, pattern_notes.size());
    for (const auto &vector : pattern_notes) {
        combine_hash(seed, vector.size());
        for (uint8_t byte : vector) {
            combine_hash(seed, byte);
        }
    }

    std::stringstream stream;
    stream << std::hex << seed;
    hash = stream.str();
}

void ClipboardNotes::generate_name() {
    if (pattern_notes.empty()) {
        name = "Empty notes selection";
        return;
    }

    int count = 0;
    std::stringstream stream;
    const size_t size = pattern_notes.size();
    stream << size << " channel" << (size > 1 ? "s" : "") << ": ";
    for (size_t channel = 0; channel < pattern_notes.size(); ++channel) {
        const auto &notes = pattern_notes[channel];
        for (int i = 0; i < notes.size(); ++i) {
            if (count > MAX_CLIPBOARD_ENUMERATION) {
                stream << ", ...";
                name = stream.str();
                return;
            }

            ++count;
            stream << get_full_note_name(notes[i]);
            if (i < notes.size() - 1 || channel < pattern_notes.size() - 1) {
                stream << ", ";
            }
        }
    }

    name = stream.str();
}
