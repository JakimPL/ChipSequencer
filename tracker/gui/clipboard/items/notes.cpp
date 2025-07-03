#include <sstream>

#include "../../../utils/math.hpp"
#include "../../names.hpp"
#include "notes.hpp"

ClipboardNotes::ClipboardNotes(const PatternNotes &notes)
    : ClipboardItem(ClipboardCategory::Notes), pattern_notes(notes) {
    generate_hash();
    generate_name();
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
        name = "Empty pattern notes selection";
        return;
    }

    int count = 0;
    std::stringstream stream;
    stream << "Notes (" << pattern_notes.size() << " channels): ";
    for (size_t channel = 0; channel < pattern_notes.size(); ++channel) {
        const auto &notes = pattern_notes[channel];
        for (int i = 0; i < notes.size(); ++i) {
            if (count > MAX_CLIPBOARD_ENUMERATION) {
                stream << ", ...";
                name = stream.str();
                return;
            }

            ++count;
            stream << get_full_note_name(notes[0]);
            if (i < notes.size() - 1 || channel < pattern_notes.size() - 1) {
                stream << ", ";
            }
        }
    }

    name = stream.str();
}
