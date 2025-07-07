#pragma once

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

template <typename T>
void write_data(std::ofstream &file, const T *data, size_t size);

template <typename T>
void read_data(std::ifstream &file, T *data, size_t size);

template <typename T>
void write_vector(std::ofstream &file, const std::vector<T> &vector, const std::vector<size_t> &sizes, bool write_count);

template <typename T>
void export_data(const std::string &filename, const T &data, size_t size);

template <typename T>
void export_vector(const std::string &filename, const std::vector<T> &vector, const std::vector<size_t> &sizes);

template <typename T>
std::vector<size_t> get_struct_sizes(const std::vector<T> &data);

size_t get_struct_size(const void *pointer);

nlohmann::json read_json(const std::filesystem::path &filename);
void save_json(const std::filesystem::path &filename, const nlohmann::json &json);
