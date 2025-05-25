#ifndef MAPS_ROUTING_HPP
#define MAPS_ROUTING_HPP

#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "../constants.hpp"
#include "../structures.hpp"
#include "../song/links/target.hpp"

typedef std::tuple<std::vector<size_t>, std::vector<std::string>, std::vector<uint16_t>, std::vector<TargetVariableType>> RoutingTuple;

struct RoutingItem {
    std::string label;
    uint16_t offset;
    TargetVariableType type;
    int constraint = -1;
    int default_shift = 0;
};

struct RoutingItems {
    RoutingItems(std::vector<RoutingItem> items);

    std::vector<std::string> labels;
    std::vector<uint16_t> offsets;
    std::vector<TargetVariableType> types;
    std::vector<int> constraints;
    std::map<uint16_t, size_t> offset_to_index;

    RoutingTuple filter_items(const int index) const;
};

extern const std::map<Target, RoutingItems> routing_variables;

#endif // MAPS_ROUTING_HPP
