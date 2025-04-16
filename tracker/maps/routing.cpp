#include "routing.hpp"

RoutingItems::RoutingItems(
    std::vector<std::string> labels,
    std::vector<uint16_t> offsets,
    std::vector<TargetVariableType> types,
    std::vector<int> constraints
)
    : labels(labels), offsets(offsets), types(types), constraints(constraints) {
    for (size_t i = 0; i < offsets.size(); ++i) {
        offset_to_index[offsets[i]] = i;
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
