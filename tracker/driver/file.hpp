#ifndef DRIVER_FILE_HPP
#define DRIVER_FILE_HPP

#include <fstream>
#include <iostream>
#include "driver.hpp"

class FileDriver : public Driver {
  public:
    FileDriver(
        const std::array<t_output, SONG_LENGTH> &target,
        const std::string &filename)
        : Driver(target), filename(filename) {}

    bool initialize() override {
        return true;
    }

    void play() override {
        save_output_to_file();
    }

  private:
    void save_output_to_file() {
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

    std::string filename;
};

#endif // DRIVER_FILE_HPP