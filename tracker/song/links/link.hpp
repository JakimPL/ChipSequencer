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
    uint16_t id;
    Target target;
    uint8_t index;
    uint16_t offset;

    void *item = nullptr;
    void *base = nullptr;
    void *pointer = nullptr;
    uint8_t table_id = -1;
    LinkKey key;

    void assign_output();
    void serialize(std::ofstream &file) const;
    void deserialize(std::ifstream &file);
};

typedef std::array<std::vector<Link>, static_cast<size_t>(ItemType::COUNT)> Links;
typedef std::map<uint8_t, std::map<uint8_t, Link>> CommandsLinks;

#endif // SONG_LINKS_LINK_HPP
