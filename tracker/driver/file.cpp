#include "file.hpp"
#include <fstream>
#include <iostream>

FileDriver::FileDriver(const std::string &filename) {
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
        // TODO: not implemented yet
        file.close();
    } else {
        std::cerr << "Unable to open file" << std::endl;
    }
}
