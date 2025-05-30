#include <fstream>
#include <iostream>

#include "../audio/wave.hpp"
#include "../song/core.hpp"
#include "../song/functions.hpp"
#include "file.hpp"

FileDriver::FileDriver() {
}

void FileDriver::terminate() {
    return;
}

void FileDriver::play() {
    samples.clear();
    samples.reserve(song_length);
    initialize();
    for (size_t i = 0; i < song_length; i++) {
        frame();
        std::vector<float> frame;
        for (size_t j = 0; j < output_channels; j++) {
            frame.push_back(output[j]);
        }
        samples.push_back(frame);
    }

    save_output_to_file();
    std::cout << "Render saved to " << filename << std::endl;
}

void FileDriver::set_length(const size_t length) {
    song_length = length;
}

void FileDriver::set_sample_rate(const int rate) {
    sample_rate = rate;
}

void FileDriver::set_output_channels(const int channels) {
    output_channels = channels;
}

void FileDriver::set_output_filename(const std::string &filename) {
    this->filename = filename;
}

void FileDriver::save_output_to_file() {
    save_wave(filename, samples, sample_rate, output_channels);
}

void FileDriver::set(const std::string &filename, const int rate, const int channels, const size_t length) {
    set_length(length);
    set_sample_rate(rate);
    set_output_channels(channels);
    set_output_filename(filename);
}
