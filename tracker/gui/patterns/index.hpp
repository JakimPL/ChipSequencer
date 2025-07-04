#pragma once

#include <cstdint>
#include <tuple>

struct Pattern;
struct CommandsPattern;

struct VariantChannelIndex {
    bool command;
    size_t index;

    bool operator<(const VariantChannelIndex &other) const {
        return std::tie(command, index) < std::tie(other.command, other.index);
    }
};

struct PatternIndex {
    Pattern *pattern;
    size_t pattern_id;
    uint16_t index;

    bool operator<(const PatternIndex &other) const {
        return std::tie(pattern_id, index) < std::tie(other.pattern_id, other.index);
    }
};

struct ConstPatternIndex {
    const Pattern *pattern;
    const size_t pattern_id;
    const uint16_t index;

    bool operator<(const ConstPatternIndex &other) const {
        return std::tie(pattern_id, index) < std::tie(other.pattern_id, other.index);
    }
};

struct CommandsPatternIndex {
    CommandsPattern *pattern;
    size_t pattern_id;
    uint16_t index;

    bool operator<(const CommandsPatternIndex &other) const {
        return std::tie(pattern_id, index) < std::tie(other.pattern_id, other.index);
    }
};

struct ConstCommandsPatternIndex {
    const CommandsPattern *pattern;
    const size_t pattern_id;
    const uint16_t index;

    bool operator<(const ConstCommandsPatternIndex &other) const {
        return std::tie(pattern_id, index) < std::tie(other.pattern_id, other.index);
    }
};
