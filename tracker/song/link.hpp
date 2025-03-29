#ifndef SONG_LINK_HPP
#define SONG_LINK_HPP

#include <cstdint>
#include <map>
#include <vector>

#include "target.hpp"

enum class ItemType : u_int8_t {
    CHANNEL = 0,
    DSP = 1
};

struct Link {
    ItemType type;
    uint8_t id;
    Target target;
    uint8_t index;
    uint16_t offset;
    void *item = nullptr;
    void *base = nullptr;
    void *pointer = nullptr;

    void assign_output();
    void serialize(std::ofstream &file) const;
    void deserialize(std::ifstream &file);
};

typedef std::vector<std::vector<Link>> Links;

#endif // SONG_LINK_HPP
