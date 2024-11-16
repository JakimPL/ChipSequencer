#include <array>
#include <iostream>
#include <fstream>
#include "song.hpp"

extern "C" {
void sound_driver_initialize() __attribute__((used));
void sound_driver_terminate() __attribute__((used));
void sound_driver_step() __attribute__((used));
void initialize();
void mix();
}

void sound_driver_initialize() {
    return;
}

void sound_driver_terminate() {
    return;
}

void sound_driver_step() {
    return;
}

void save_output_to_file(const std::string &filename, const std::array<t_output, SONG_LENGTH> &output) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto &value : output) {
            file << value << std::endl;
        }
        file.close();
    } else {
        std::cerr << "Unable to open file" << std::endl;
    }
}

int main() {
    std::cout << "Starting the program..." << std::endl;
    initialize();
    std::cout << "ChipSequencer initialized!" << std::endl;

    std::array<t_output, SONG_LENGTH> target;
    for (uint i = 0; i < SONG_LENGTH; ++i) {
        mix();
        target[i] = output;
    }

    save_output_to_file("output.txt", target);

    return 0;
}
