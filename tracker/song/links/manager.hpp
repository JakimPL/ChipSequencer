#ifndef SONG_LINKS_MANAGER_HPP
#define SONG_LINKS_MANAGER_HPP

#include <cstdint>
#include <variant>
#include <vector>
#include <unordered_map>

#include "key.hpp"
#include "link.hpp"

using ParameterValue = std::variant<uint8_t, uint16_t, uint32_t, _Float32>;

class LinkManager {
  private:
    std::unordered_map<LinkKey, std::vector<Link *>> map;
    std::unordered_map<LinkKey, ParameterValue> snapshot;
    std::vector<std::pair<void *, LinkKey>> pointers_map;

    void capture_parameter(const LinkKey key, const Link *link);
    void restore_parameter(const LinkKey key, const Link *link) const;
    void validate_key_and_link(const LinkKey key, const Link *link) const;
    TargetVariableType get_type(const LinkKey key) const;

    void remove_key(Link &link);
    void assign_key(Link &link);
    void clear();

  public:
    void reset();
    void set_link(Link &link, void *item, const uint8_t i);
    void set_links();
    void save_targets();

    void realign_links(const size_t index, const Target target, const ItemType type);
    void realign_links(const size_t index, const Target target);

    bool is_linked(const LinkKey key) const;
    std::vector<Link *> get_links(const LinkKey key) const;
    std::string get_link_reference(const LinkKey key) const;
    std::string get_link_reference(const ItemType type, const size_t index) const;
    std::vector<std::pair<void *, LinkKey>> get_pointers_map() const;
    std::pair<void *, LinkKey> get_pointer_and_key(size_t index) const;

    void capture_parameters();
    void restore_parameters() const;
};

#endif // SONG_LINKS_MANAGER_HPP
