#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "item.hpp"

using PatternNotes = std::vector<std::vector<uint8_t>>;

struct ClipboardNotes : public ClipboardItem {
    ClipboardNotes(const PatternNotes &notes);
    PatternNotes pattern_notes;

    void generate_hash() override;
    void generate_name() override;
};
