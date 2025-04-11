#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include <string>

std::string to_lower(const std::string &string);
void string_copy_to_buffer(const std::string &string, char *target, size_t size);

#endif // UTILS_STRING_HPP
