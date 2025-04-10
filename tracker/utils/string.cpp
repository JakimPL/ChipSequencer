#include <algorithm>
#include <cctype>

#include "string.hpp"

std::string to_lower(const std::string &string) {
    std::string result = string;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}
