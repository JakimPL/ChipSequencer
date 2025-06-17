#pragma once

#include <string>

template <size_t n>
struct TextChange {
    char (&buffer)[n];
    std::string old_value;
    std::string new_value;

    TextChange(char (&buf)[n], const std::string &old, const std::string &new_val)
        : buffer(buf), old_value(old), new_value(new_val) {}
};
