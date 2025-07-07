#include "../structures.hpp"
#include "../song/links/type.hpp"
#include "../gui/constants.hpp"
#include "file.hpp"

template <typename T>
void write_data(std::ofstream &file, const T *data, const size_t size) {
    file.write(reinterpret_cast<const char *>(data), size);
}

template <typename T>
void read_data(std::ifstream &file, T *data, const size_t size) {
    file.read(reinterpret_cast<char *>(data), size);
    if (!file) {
        throw std::runtime_error("Failed to read data from file. Bytes read: " + std::to_string(file.gcount()) + " out of " + std::to_string(size));
    }
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

size_t get_struct_size(const void *pointer) {
    const uint8_t *size_pointer = static_cast<const uint8_t *>(pointer);
    return static_cast<size_t>(*size_pointer);
}

template <typename T>
std::vector<size_t> get_struct_sizes(const std::vector<T> &data) {
    std::vector<size_t> sizes;
    sizes.reserve(data.size());
    for (const void *pointer : data) {
        sizes.push_back(get_struct_size(pointer) + 1);
    }
    return sizes;
}

nlohmann::json read_json(const std::filesystem::path &filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename.string());
    }
    nlohmann::json json;
    file >> json;
    return json;
}

void save_json(const std::filesystem::path &filename, const nlohmann::json &json) {
    std::ofstream file(filename);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + filename.string());
    }
    file << json.dump(JSON_INDENTATION);
    file.close();
}

template void write_data<uint8_t>(std::ofstream &file, const uint8_t *data, const size_t size);
template void write_data<uint16_t>(std::ofstream &file, const uint16_t *data, const size_t size);
template void write_data<uint32_t>(std::ofstream &file, const uint32_t *data, const size_t size);
template void write_data<uint64_t>(std::ofstream &file, const uint64_t *data, const size_t size);
template void write_data<int8_t>(std::ofstream &file, const int8_t *data, const size_t size);
template void write_data<int16_t>(std::ofstream &file, const int16_t *data, const size_t size);
template void write_data<int32_t>(std::ofstream &file, const int32_t *data, const size_t size);
template void write_data<int64_t>(std::ofstream &file, const int64_t *data, const size_t size);
template void write_data<float>(std::ofstream &file, const float *data, const size_t size);
template void write_data<double>(std::ofstream &file, const double *data, const size_t size);

template void read_data<uint8_t>(std::ifstream &file, uint8_t *data, const size_t size);
template void read_data<uint16_t>(std::ifstream &file, uint16_t *data, const size_t size);
template void read_data<uint32_t>(std::ifstream &file, uint32_t *data, const size_t size);
template void read_data<uint64_t>(std::ifstream &file, uint64_t *data, const size_t size);
template void read_data<int8_t>(std::ifstream &file, int8_t *data, const size_t size);
template void read_data<int16_t>(std::ifstream &file, int16_t *data, const size_t size);
template void read_data<int32_t>(std::ifstream &file, int32_t *data, const size_t size);
template void read_data<int64_t>(std::ifstream &file, int64_t *data, const size_t size);
template void read_data<float>(std::ifstream &file, float *data, const size_t size);
template void read_data<double>(std::ifstream &file, double *data, const size_t size);

template void write_data<ItemType>(std::ofstream &file, const ItemType *data, const size_t size);
template void write_data<Target>(std::ofstream &file, const Target *data, const size_t size);
template void write_data<Channel>(std::ofstream &file, const Channel *data, const size_t size);
template void write_data<Command>(std::ofstream &file, const Command *data, const size_t size);
template void write_data<Note>(std::ofstream &file, const Note *data, const size_t size);

template void read_data<ItemType>(std::ifstream &file, ItemType *data, const size_t size);
template void read_data<Target>(std::ifstream &file, Target *data, const size_t size);
template void read_data<Envelope>(std::ifstream &file, Envelope *data, const size_t size);
template void read_data<CommandsChannel>(std::ifstream &file, CommandsChannel *data, const size_t size);
template void read_data<Command>(std::ifstream &file, Command *data, const size_t size);
template void read_data<Note>(std::ifstream &file, Note *data, const size_t size);

template void write_data<uint8_t[0]>(std::ofstream &file, const uint8_t (*data)[0], const size_t size);
template void write_data<uint8_t[1]>(std::ofstream &file, const uint8_t (*data)[1], const size_t size);
template void write_data<uint8_t[2]>(std::ofstream &file, const uint8_t (*data)[2], const size_t size);
template void write_data<uint8_t[4]>(std::ofstream &file, const uint8_t (*data)[4], const size_t size);

template void read_data<uint8_t[4]>(std::ifstream &file, uint8_t (*data)[4], const size_t size);

template void export_data<Envelope *>(const std::string &filename, Envelope *const &data, const size_t size);
template void export_data<void *>(const std::string &filename, void *const &data, const size_t size);
template void export_data<CommandsChannel *>(const std::string &filename, CommandsChannel *const &data, const size_t size);

template std::vector<size_t> get_struct_sizes<void *>(const std::vector<void *> &data);
