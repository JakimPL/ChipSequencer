#include "file.hpp"
#include <fstream>
#include <iostream>

FileDriver::FileDriver(
    const std::array<t_output, SONG_LENGTH> &target,
    const std::string &filename
)
    : Driver(target), filename(filename) {
}

bool FileDriver::initialize() {
    return true;
}

void FileDriver::terminate() {
    return;
}

void FileDriver::play() {
    save_output_to_file();
}

void FileDriver::save_output_to_file() {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto &value : target) {
            file << value << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file" << std::endl;
    }
}
