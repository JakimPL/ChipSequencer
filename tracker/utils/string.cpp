#include <algorithm>
#include <iomanip>
#include <cctype>
#include <cstring>
#include <sstream>

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

std::vector<std::string> split(const std::string &string, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream stream(string);
    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string convert_double_to_string(const double value, const uint8_t decimals) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimals) << value;
    return stream.str();
}

void pad_string(std::string &string, const char padding, const size_t size) {
    if (string.size() < size) {
        string.resize(size, padding);
    }
}
