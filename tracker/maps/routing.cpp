#include "routing.hpp"

RoutingItems::RoutingItems(
    std::vector<std::string> labels,
    std::vector<uint16_t> offsets,
    std::vector<TargetVariableType> types
)
    : labels(labels), offsets(offsets), types(types) {
    for (size_t i = 0; i < offsets.size(); ++i) {
        offset_to_index[offsets[i]] = i;
    }
}
