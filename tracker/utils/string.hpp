#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <string>
#include <vector>

std::string to_lower(const std::string &string);
void string_copy_to_buffer(const std::string &string, char *target, size_t size);
std::vector<std::string> split(const std::string &string, const char delimiter);
std::string convert_double_to_string(const double value, const uint8_t decimals);

#endif // UTILS_STRING_HPP
