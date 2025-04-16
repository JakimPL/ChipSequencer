#ifndef GUI_NODE_HPP
#define GUI_NODE_HPP

#include <optional>
#include <string>
#include <vector>

#include "../imgui/imgui.h"

#include "../song/links/key.hpp"
#include "../song/links/type.hpp"

typedef std::pair<ItemType, int> InputKey;
typedef LinkKey OutputKey;

struct NodeIdentifier {
    Target type;
    size_t id;

    bool operator==(const NodeIdentifier &other) const {
        return type == other.type && id == other.id;
    }
};

template <>
struct std::hash<NodeIdentifier> {
    std::size_t operator()(const NodeIdentifier &k) const {
        return (std::hash<int>()(static_cast<int>(k.type)) ^ (std::hash<size_t>()(k.id) << 1));
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
};

#endif // GUI_NODE_HPP
