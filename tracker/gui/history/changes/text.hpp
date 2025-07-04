#pragma once

#include <string>

template <size_t n>
struct TextChange {
    char (&buffer)[n];
    std::string old_value;
    std::string new_value;

    TextChange(char (&buf)[n], std::string old_v, std::string new_v)
        : buffer(buf), old_value(old_v), new_value(new_v) {}
};
