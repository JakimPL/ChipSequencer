#ifndef SONG_LINKS_MANAGER_HPP
#define SONG_LINKS_MANAGER_HPP

#include <cstdint>

#include "link.hpp"

struct LinkManager {
    void set_link(Link &link, void *item, const uint8_t i) const;
    void set_links() const;
    void realign_links(const size_t index, const Target target, const ItemType type) const;
    void realign_links(const size_t index, const Target target) const;
};

#endif // SONG_LINKS_MANAGER_HPP
