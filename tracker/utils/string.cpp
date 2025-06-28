#include <algorithm>
#include <iomanip>
#include <cctype>
#include <cstring>
#include <sstream>

#include "math.hpp"
#include "string.hpp"

std::string to_lower(const std::string &string) {
    std::string result = string;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
    return result;
}

void copy_string_to_buffer(const std::string &string, char *target, size_t size) {
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

std::string convert_double_to_string(const double value, const char *format) {
    char buffer[64];
    std::snprintf(buffer, sizeof(buffer), format, value);
    return std::string(buffer);
}

void pad_string(std::string &string, const char padding, const size_t size) {
    if (string.size() < size) {
        string.resize(size, padding);
    }
}

int string_to_integer(const std::string &string, const int default_value, const int min, const int max) {
    try {
        int value = std::stoi(string);
        return clamp(value, min, max);
    } catch (const std::invalid_argument &) {
        return default_value;
    } catch (const std::out_of_range &) {
        return default_value;
    }
}

double string_to_double(const std::string &string, const double default_value, const double min, const double max) {
    try {
        double value = std::stod(string);
        return clamp(value, min, max);
    } catch (const std::invalid_argument &) {
        return default_value;
    } catch (const std::out_of_range &) {
        return default_value;
    }
}

bool compare_floats(const float a, const float b, const char *format) {
    char buffer_a[64];
    char buffer_b[64];

    std::snprintf(buffer_a, sizeof(buffer_a), format, a);
    std::snprintf(buffer_b, sizeof(buffer_b), format, b);

    return std::strcmp(buffer_a, buffer_b) == 0;
}
