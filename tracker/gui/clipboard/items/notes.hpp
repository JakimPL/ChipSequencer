#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "item.hpp"

typedef std::vector<std::vector<uint8_t>> PatternNotes;

struct ClipboardNotes : public ClipboardItem {
    ClipboardNotes(const std::string &nm, const PatternNotes &notes);
    PatternNotes pattern_notes;

    std::string generate_hash() const override;
};
