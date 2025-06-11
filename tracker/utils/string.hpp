#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <string>
#include <vector>

#define DEFAULT_FLOAT_PRECISION 6

std::string to_lower(const std::string &string);
void copy_string_to_buffer(const std::string &string, char *target, size_t size);
std::vector<std::string> split(const std::string &string, const char delimiter);
std::string convert_double_to_string(const double value, const uint8_t decimals = DEFAULT_FLOAT_PRECISION);
void pad_string(std::string &string, const char padding, const size_t size);
int string_to_integer(
    const std::string &string,
    const int default_value = 0,
    const int min = std::numeric_limits<int32_t>::min(),
    const int max = std::numeric_limits<int32_t>::max()
);
double string_to_double(
    const std::string &string,
    const double default_value = 0.0,
    const double min = -std::numeric_limits<double>::infinity(),
    const double max = std::numeric_limits<double>::infinity()
);

#endif // UTILS_STRING_HPP
