#ifndef UTILS_TEMP_HPP
#define UTILS_TEMP_HPP

#include <filesystem>
#include <utility>

std::pair<std::filesystem::path, std::filesystem::path> prepare_temp_directory();

#endif // UTILS_TEMP_HPP
