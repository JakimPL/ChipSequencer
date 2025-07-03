#pragma once

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "../song/links/key.hpp"
#include "../song/links/target.hpp"

enum RoutingConstraints {
    ROUTING_HIDDEN = -2,
    ROUTING_NO_CONSTRAINTS = -1,
};

using RoutingTuple = std::tuple<std::vector<size_t>, std::vector<std::string>, std::vector<uint16_t>, std::vector<TargetVariableType>>;

struct RoutingItem {
    std::string label;
    uint16_t offset;
    TargetVariableType type;
    int constraint = ROUTING_NO_CONSTRAINTS;
    int default_shift = 0;
};

struct RoutingItems {
    RoutingItems(std::vector<RoutingItem> items);

    std::vector<std::string> labels;
    std::vector<uint16_t> offsets;
    std::vector<TargetVariableType> types;
    std::vector<int> constraints;
    std::map<std::pair<int, uint16_t>, size_t> offset_to_index;

    RoutingTuple filter_items(int constraint, bool allow_hidden = false) const;
    size_t get_index_from_offset(LinkKey key, bool allow_hidden = false) const;
};

std::string get_key_name(const LinkKey key);

extern const std::map<Target, RoutingItems> routing_variables;
