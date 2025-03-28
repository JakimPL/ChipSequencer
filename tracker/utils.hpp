#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <vector>

template <typename T>
void write_data(std::ofstream &file, const T *data, const size_t size) {
    file.write(reinterpret_cast<const char *>(data), size);
}

template <typename T>
void write_vector(std::ofstream &file, const std::vector<T> &vector, const std::vector<size_t> &sizes, bool write_count) {
    size_t count = vector.size();
    if (write_count) {
        write_data(file, &count, 1);
    }
    for (size_t i = 0; i < count; i++) {
        write_data(file, vector[i], sizes[i % sizes.size()]);
    }
}

template <typename T>
void export_data(const std::string &filename, const T &data, const size_t size) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + filename);
    }

    write_data(file, data, size);
    file.close();
}

template <typename T>
void export_vector(const std::string &filename, const std::vector<T> &vector, const std::vector<size_t> &sizes) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + filename);
    }

    write_vector(file, vector, sizes, false);
    file.close();
}

inline size_t get_struct_size(const void *pointer) {
    const uint8_t *size_pointer = static_cast<const uint8_t *>(pointer);
    return static_cast<size_t>(*size_pointer);
}

template <typename T>
std::vector<size_t> get_struct_sizes(const std::vector<T> &data) {
    std::vector<size_t> sizes;
    for (const void *pointer : data) {
        sizes.push_back(get_struct_size(pointer) + 1);
    }
    return sizes;
}

#endif // UTILS_HPP
