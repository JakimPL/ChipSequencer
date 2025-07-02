#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "item.hpp"

typedef std::vector<std::vector<uint8_t>> PatternNotes;

struct PatternNotesClipboardItem : public ClipboardItem {
    PatternNotesClipboardItem(const std::string &nm);
    PatternNotes pattern_notes;

    std::string generate_hash() const override;
};
