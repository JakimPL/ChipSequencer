#ifndef SONG_LINKS_MANAGER_HPP
#define SONG_LINKS_MANAGER_HPP

#include <cstdint>
#include <unordered_map>
#include <variant>
#include <vector>

#include "key.hpp"
#include "link.hpp"

using ParameterValue = std::variant<uint8_t, uint16_t, uint32_t, _Float32>;

class LinkManager {
  private:
    std::unordered_map<LinkKey, std::vector<Link *>> map;
    std::unordered_map<LinkKey, ParameterValue> snapshot;
    std::vector<std::pair<void *, LinkKey>> pointers_map;

    void set_channels_links();
    void set_dsps_links();
    void set_commands_links();

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

    bool is_linked(const LinkKey key) const;
    std::vector<Link *> get_links(const LinkKey key) const;
    std::string get_link_reference(const LinkKey key) const;
    std::string get_link_reference(const ItemType type, const size_t index) const;
    std::vector<std::pair<void *, LinkKey>> get_pointers_map() const;
    std::pair<void *, LinkKey> get_pointer_and_key(size_t index) const;
    size_t find_pointer_id_by_key(const LinkKey key) const;

    std::vector<std::pair<ItemType, uint8_t>> find_dependencies(const Target target, const size_t index) const;
    void remove_dependencies(const Target target, const size_t index);
    void realign_links(const Target target, const size_t index, const ItemType type);
    void realign_links(const Target target, const size_t index);

    void capture_parameters();
    void restore_parameters() const;

    static std::pair<uint8_t, uint8_t> unpack_command_id(const uint16_t id);
};

#endif // SONG_LINKS_MANAGER_HPP
