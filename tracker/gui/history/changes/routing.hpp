#pragma once

#include "../../../gui/node.hpp"

struct RoutingChange {
    InputKey input_key;
    OutputKey old_routing;
    OutputKey new_routing;

    RoutingChange(InputKey input, const OutputKey &old_routing, const OutputKey &new_routing)
        : input_key(input), old_routing(old_routing), new_routing(new_routing) {}
};
