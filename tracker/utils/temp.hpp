#ifndef UTILS_TEMP_HPP
#define UTILS_TEMP_HPP

#include <filesystem>
#include <utility>

std::pair<std::filesystem::path, std::filesystem::path> prepare_temp_directory(const bool clean_temp);
void remove_temp_directory(const std::filesystem::path &directory, const bool clear_temp);

#endif // UTILS_TEMP_HPP
