#include <sstream>

#include "../../../utils/math.hpp"
#include "notes.hpp"

PatternNotesClipboardItem::PatternNotesClipboardItem(const std::string &nm)
    : ClipboardItem(ClipboardCategory::Pattern, nm, generate_hash()) {
    ;
}

std::string PatternNotesClipboardItem::generate_hash() const {
    size_t seed = 0;

    combine_hash(seed, pattern_notes.size());

    for (const auto &inner_vec : pattern_notes) {
        combine_hash(seed, inner_vec.size());
        for (uint8_t byte : inner_vec) {
            combine_hash(seed, byte);
        }
    }

    std::stringstream stream;
    stream << std::hex << seed;
    return stream.str();
}
