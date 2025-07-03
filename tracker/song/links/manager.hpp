#pragma once

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

    void capture_parameter(LinkKey key, const Link *link);
    void restore_parameter(LinkKey key, const Link *link) const;
    void validate_key_and_link(LinkKey key, const Link *link) const;
    TargetVariableType get_type(LinkKey key) const;

    void remove_key(Link &link);
    void assign_key(Link &link);
    void clear();

  public:
    LinkManager() = default;
    ~LinkManager() = default;
    LinkManager(const LinkManager &) = delete;
    LinkManager &operator=(const LinkManager &) = delete;
    LinkManager(LinkManager &&) = delete;
    LinkManager &operator=(LinkManager &&) = delete;

    void reset();
    void set_link(Link &link, void *item, uint8_t i);
    void set_links();
    void save_targets();

    bool is_linked(LinkKey key) const;
    std::vector<Link *> get_links(LinkKey key) const;
    std::string get_link_reference(LinkKey key) const;
    std::string get_link_reference(ItemType type, size_t index) const;
    std::vector<std::pair<void *, LinkKey>> get_pointers_map() const;
    std::pair<void *, LinkKey> get_pointer_and_key(size_t index) const;
    size_t find_pointer_id_by_key(LinkKey key) const;

    std::vector<std::pair<ItemType, uint8_t>> find_dependencies(Target target, size_t index) const;
    void remove_dependencies(Target target, size_t index);
    void realign_links(Target target, size_t index, bool removal, ItemType type);
    void realign_links(Target target, size_t index, bool removal);

    void capture_parameters();
    void restore_parameters() const;

    static std::pair<uint8_t, uint8_t> unpack_command_id(const uint16_t id);
};
