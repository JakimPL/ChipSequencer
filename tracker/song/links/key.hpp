#ifndef SONG_LINKS_KEY_HPP
#define SONG_LINKS_KEY_HPP

#include <cstdint>
#include <unordered_map>

#include "target.hpp"

struct LinkKey {
    Target target = Target::UNUSED;
    size_t index = -1;
    uint16_t offset = -1;

    bool operator==(const LinkKey &other) const {
        return target == other.target && index == other.index && offset == other.offset;
    }
};

template <>
struct std::hash<LinkKey> {
    std::size_t operator()(const LinkKey &key) const {
        std::size_t h1 = std::hash<int>()(static_cast<int>(key.target));
        std::size_t h2 = std::hash<size_t>()(key.index);
        std::size_t h3 = std::hash<uint16_t>()(key.offset);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

#endif // SONG_LINKS_KEY_HPP
