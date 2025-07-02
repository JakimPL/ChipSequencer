#include <sstream>

#include "../../../utils/math.hpp"
#include "notes.hpp"

ClipboardNotes::ClipboardNotes(const std::string &nm, const PatternNotes &notes)
    : ClipboardItem(ClipboardCategory::Pattern, nm),
      pattern_notes(notes) {
    generate_hash();
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
