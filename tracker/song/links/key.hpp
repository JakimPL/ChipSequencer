#pragma once

#include <cstdint>
#include <unordered_map>

#include "target.hpp"

struct LinkKey {
    Target target = Target::COUNT;
    int index = -1;
    uint16_t offset = -1;

    bool operator==(const LinkKey &other) const {
        return target == other.target && index == other.index && offset == other.offset;
    }

    bool operator!=(const LinkKey &other) const {
        return !(*this == other);
    }

    bool operator<(const LinkKey &other) const {
        return std::tie(target, index, offset) < std::tie(other.target, other.index, other.offset);
    }

    bool is_null() const {
        return target == Target::COUNT && index == -1 && offset == static_cast<uint16_t>(-1);
    }
};

template <>
struct std::hash<LinkKey> {
    std::size_t operator()(const LinkKey &key) const {
        std::size_t h1 = std::hash<int>()(static_cast<int>(key.target));
        std::size_t h2 = std::hash<int>()(key.index);
        std::size_t h3 = std::hash<uint16_t>()(key.offset);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};
