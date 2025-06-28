#pragma once

#include <filesystem>
#include <utility>

std::pair<std::filesystem::path, std::filesystem::path> prepare_temp_directory(const bool clean_temp);
void remove_temp_directory(const std::filesystem::path &directory, const bool clear_temp);

std::filesystem::path get_base_path();
std::filesystem::path get_executable_directory();
std::filesystem::path get_resource_path(const std::string &resource);
std::filesystem::path get_core_path();
std::filesystem::path get_scripts_path();
