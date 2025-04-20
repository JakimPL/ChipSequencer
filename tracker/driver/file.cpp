#include <fstream>
#include <iostream>
#include <sndfile.h>

#include "../song/data.hpp"
#include "../song/functions.hpp"
#include "file.hpp"

#define IEEE_FLOAT 3
#define BITS_PER_SAMPLE 32

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
        mix();
        std::vector<float> frame;
        std::cout << "Frame " << i << ": ";
        for (size_t j = 0; j < output_channels; j++) {
            frame.push_back(output[j]);
            std::cout << output[j] << " ";
        }
        std::cout << std::endl;
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
    SF_INFO sfinfo;
    sfinfo.channels = output_channels;
    sfinfo.samplerate = sample_rate;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    sfinfo.frames = samples.size();
    sfinfo.sections = 0;
    sfinfo.seekable = 0;

    SNDFILE *sndfile = sf_open(filename.c_str(), SFM_WRITE, &sfinfo);
    if (!sndfile) {
        std::cerr << "Error opening output file: " << sf_strerror(sndfile) << std::endl;
        return;
    }

    std::vector<float> interleaved;
    interleaved.reserve(samples.size() * output_channels);

    for (const auto &frame : samples) {
        for (size_t c = 0; c < output_channels; ++c) {
            interleaved.push_back(c < frame.size() ? frame[c] : 0.0f);
        }
    }

    sf_count_t written = sf_writef_float(sndfile, interleaved.data(), samples.size());
    if (written != static_cast<sf_count_t>(samples.size())) {
        std::cerr << "Failed to write all frames!" << std::endl;
    }

    sf_close(sndfile);
}
