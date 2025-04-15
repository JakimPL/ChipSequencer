#ifndef SONG_LINKS_LINK_HPP
#define SONG_LINKS_LINK_HPP

#include <array>
#include <cstdint>
#include <map>
#include <vector>

#include "key.hpp"
#include "target.hpp"
#include "type.hpp"

struct Link {
    ItemType type;
    uint8_t id;
    Target target;
    uint8_t index;
    uint16_t offset;
    void *item = nullptr;
    void *base = nullptr;
    void *pointer = nullptr;
    LinkKey key;

    void assign_output();
    void serialize(std::ofstream &file) const;
    void deserialize(std::ifstream &file);
};

typedef std::array<std::vector<Link>, 2> Links;

#endif // SONG_LINKS_LINK_HPP
