#pragma once

#include "../../gui/node.hpp"
#include "../../song/links/key.hpp"

template <typename T>
struct ValueChange {
    T &parameter;
    T old_value;
    T new_value;

    ValueChange(T &param, const T &old)
        : parameter(param), old_value(old), new_value(param) {}
};

struct RoutingChange {
    InputKey input_key;
    OutputKey old_routing;
    OutputKey new_routing;

    RoutingChange(const InputKey &input, const OutputKey &old_routing, const OutputKey &new_routing)
        : input_key(input), old_routing(old_routing), new_routing(new_routing) {}
};

struct OrderSequenceChange {
    size_t sequence_index;
    size_t old_sequence;
    size_t new_sequence;
};

template <size_t n>
struct TextChange {
    char (&buffer)[n];
    std::string old_value;
    std::string new_value;

    TextChange(char (&buf)[n], const std::string &old, const std::string &new_val)
        : buffer(buf), old_value(old), new_value(new_val) {}
};
