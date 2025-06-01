#include <sstream>
#include <vector>

#include "../utils/string.hpp"
#include "names.hpp"
#include "node.hpp"

NodeIdentifier::NodeIdentifier()
    : type(Target::COUNT), id(0) {
}

NodeIdentifier::NodeIdentifier(Target type, size_t id)
    : type(type), id(id) {
}

NodeIdentifier::NodeIdentifier(const std::string &string) {
    const std::vector<std::string> items = split(string, ':');
    if (items.size() != 2) {
        throw std::invalid_argument("Invalid node identifier format: " + string);
    }

    const std::string target_string = items[0];
    const std::string id_string = items[1];

    for (const auto &pair : target_names) {
        if (pair.second == target_string) {
            type = pair.first;
            break;
        }
    }

    if (type == Target::COUNT) {
        throw std::invalid_argument("Unknown target type: " + target_string);
    }

    id = static_cast<size_t>(string_to_integer(id_string));
}

bool NodeIdentifier::operator==(const NodeIdentifier &other) const {
    if (id != other.id) {
        return false;
    }

    bool same_target = type == other.type;
    if (type == Target::DIRECT_DSP || type == Target::DSP) {
        same_target |= other.type == Target::DIRECT_DSP || other.type == Target::DSP;
    }
    return same_target;
}

bool NodeIdentifier::operator<(const NodeIdentifier &other) const {
    return std::tie(type, id) < std::tie(other.type, other.id);
}

std::string NodeIdentifier::to_string() const {
    std::ostringstream stream;
    stream << target_names.at(type) << ":" << id;
    return stream.str();
}
