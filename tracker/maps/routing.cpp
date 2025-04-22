#include "routing.hpp"

RoutingItems::RoutingItems(std::vector<RoutingItem> items) {
    for (size_t i = 0; i < items.size(); ++i) {
        const auto &item = items[i];
        labels.push_back(item.label);
        offsets.push_back(item.offset);
        types.push_back(item.type);
        constraints.push_back(item.constraint);
        offset_to_index[item.offset] = i;
    }
}

RoutingTuple RoutingItems::filter_items(const int index) const {
    std::vector<size_t> indices;
    std::vector<std::string> filtered_labels;
    std::vector<uint16_t> filtered_offsets;
    std::vector<TargetVariableType> filtered_types;
    for (size_t i = 0; i < labels.size(); ++i) {
        if (constraints[i] == -1 || constraints[i] == index) {
            indices.push_back(i);
            filtered_labels.push_back(labels[i]);
            filtered_offsets.push_back(offsets[i]);
            filtered_types.push_back(types[i]);
        }
    }

    return {indices, filtered_labels, filtered_offsets, filtered_types};
}
