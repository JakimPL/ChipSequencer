#include <sstream>
#include <vector>

#include "../utils/string.hpp"
#include "names.hpp"
#include "node.hpp"

bool NodeIdentifier::operator==(const NodeIdentifier &other) const {
    return type == other.type && id == other.id;
}

bool NodeIdentifier::operator<(const NodeIdentifier &other) const {
    return std::tie(type, id) < std::tie(other.type, other.id);
}

std::string NodeIdentifier::serialize(const NodeIdentifier &node) const {
    if (target_names.find(node.type) == target_names.end()) {
        throw std::invalid_argument("Unknown target type");
    }

    std::ostringstream stream;
    stream << target_names.at(node.type) << ":" << node.id;
    return stream.str();
}

NodeIdentifier NodeIdentifier::deserialize(const std::string &string) {
    const std::vector<std::string> items = split(string, ':');
    if (items.size() != 2) {
        throw std::invalid_argument("Invalid node identifier format: " + string);
    }

    const std::string target_string = items[0];
    const std::string id_string = items[1];
    const size_t id = static_cast<size_t>(string_to_integer(id_string));

    Target type = Target::COUNT;
    for (const auto &pair : target_names) {
        if (pair.second == target_string) {
            type = pair.first;
            break;
        }
    }

    if (type == Target::COUNT) {
        throw std::invalid_argument("Unknown target type: " + target_string);
    }

    return NodeIdentifier{type, id};
}
