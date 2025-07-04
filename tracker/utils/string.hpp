#pragma once

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#define DEFAULT_FLOAT_PRECISION 6

std::string to_lower(const std::string &string);
void copy_string_to_buffer(const std::string &string, char *target, size_t size);
std::vector<std::string> split(const std::string &string, char delimiter);
std::string convert_double_to_string(double value, uint8_t decimals = DEFAULT_FLOAT_PRECISION);
std::string convert_double_to_string(double value, const char *format);
void pad_string(std::string &string, char padding, size_t size);
int string_to_integer(
    const std::string &string,
    int default_value = 0,
    int min = std::numeric_limits<int32_t>::min(),
    int max = std::numeric_limits<int32_t>::max()
);
double string_to_double(
    const std::string &string,
    double default_value = 0.0,
    double min = -std::numeric_limits<double>::infinity(),
    double max = std::numeric_limits<double>::infinity()
);
bool compare_floats(
    float a,
    float b,
    const char *format = "%.4f"
);
