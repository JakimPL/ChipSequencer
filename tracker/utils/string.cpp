#include <algorithm>
#include <cctype>
#include <cstring>

#include "string.hpp"

std::string to_lower(const std::string &string) {
    std::string result = string;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

void string_copy_to_buffer(const std::string &string, char *target, size_t size) {
    if (size == 0) {
        return;
    }

    std::strncpy(target, string.c_str(), size - 1);
    target[size - 1] = '\0';
}
