#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <vector>

template <typename T>
void write_data(std::ofstream &file, const T *data, const size_t size) {
    file.write(reinterpret_cast<const char *>(data), size);
}

template <typename T>
void write_vector(std::ofstream &file, const std::vector<T> &vector, const size_t size, bool write_count) {
    size_t count = vector.size();
    if (write_count) {
        write_data(file, &count, 1);
    }
    for (size_t i = 0; i < count; i++) {
        write_data(file, vector[i], size);
    }
}

template <typename T>
void export_vector(const std::string &filename, const std::vector<T> &vector, const size_t size) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + filename);
    }

    write_vector(file, vector, size, false);
    file.close();
}

template <typename T>
void export_arrays(const std::string &filename, const std::vector<T> &vector, const size_t size) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + filename);
    }

    size_t count = vector.size();
    for (size_t i = 0; i < count; i++) {
        vector[i]->serialize(file);
    }
    file.close();
}

#endif // UTILS_HPP
