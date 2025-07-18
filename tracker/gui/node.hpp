#pragma once

#include <functional>
#include <optional>
#include <string>
#include <vector>

#include "../imgui/imgui.h"

#include "../song/links/key.hpp"
#include "../song/links/type.hpp"
#include "../song/links/target.hpp"

using InputKey = std::pair<ItemType, int>;
using OutputKey = LinkKey;

struct NodeIdentifier {
    Target type = Target::COUNT;
    size_t id = -1;

    NodeIdentifier() = default;
    NodeIdentifier(Target type, size_t id);
    explicit NodeIdentifier(const std::string &string);
    bool operator==(const NodeIdentifier &other) const;
    bool operator<(const NodeIdentifier &other) const;
    std::string to_string() const;
};

template <>
struct std::hash<NodeIdentifier> {
    std::size_t operator()(const NodeIdentifier &k) const {
        return (std::hash<int>()(static_cast<int>(k.type)) ^ (std::hash<size_t>()(k.id) << 1U));
    }
};

struct RoutingNode {
    NodeIdentifier identifier;
    std::optional<InputKey> key;
    std::string name;
    std::vector<std::pair<OutputKey, std::string>> parameters;
    ImVec2 position;
    ImVec2 size;
    int lines = 1;
    bool bypass = false;
    bool solo = false;
};
