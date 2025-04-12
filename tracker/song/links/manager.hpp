#ifndef SONG_LINKS_MANAGER_HPP
#define SONG_LINKS_MANAGER_HPP

#include <cstdint>
#include <map>

#include "key.hpp"
#include "link.hpp"

class LinkManager {
  private:
    std::unordered_map<LinkKey, Link *> map;

  public:
    void set_link(Link &link, void *item, const uint8_t i);
    void set_links();
    void realign_links(const size_t index, const Target target, const ItemType type);
    void realign_links(const size_t index, const Target target);
    bool is_linked(const Target target, const size_t index, const uint16_t offset) const;

    void remove_key(Link &link);
    void assign_key(Link &link);
};

#endif // SONG_LINKS_MANAGER_HPP
