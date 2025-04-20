#include <fstream>
#include <iostream>

#include "../song/data.hpp"
#include "../song/functions.hpp"
#include "file.hpp"

#define IEEE_FLOAT 3
#define BITS_PER_SAMPLE 32

FileDriver::FileDriver() {
}

bool FileDriver::initialize() {
    samples.clear();
    return true;
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
        for (size_t j = 0; j < output_channels; j++) {
            frame.push_back(output[j]);
        }
        samples.push_back(frame);
    }
}

void FileDriver::set_length(size_t length) {
    song_length = length;
}

void FileDriver::set_sample_rate(int rate) {
    sample_rate = rate;
}

void FileDriver::set_output_channels(int channels) {
    output_channels = channels;
}

void FileDriver::set_output_filename(const std::string &filename) {
    this->filename = filename;
}

void FileDriver::save_output_to_file() {
    const uint16_t audio_format = IEEE_FLOAT;
    const uint16_t num_channels = static_cast<uint16_t>(output_channels);
    const uint32_t sample_rate_value = static_cast<uint32_t>(sample_rate);
    const uint16_t bits_per_sample = BITS_PER_SAMPLE;

    if (num_channels == 0 || sample_rate_value == 0) {
        throw std::runtime_error("Invalid audio parameters: channels or sample rate is zero.");
    }
    if (samples.empty() || samples[0].size() != num_channels) {
        throw std::runtime_error("Sample data is inconsistent with channel count.");
    }

    const uint16_t block_align = num_channels * (bits_per_sample / 8);
    const uint32_t byte_rate = sample_rate_value * block_align;
    const uint32_t total_samples = samples.size() * num_channels;
    const uint32_t subchunk2_size = total_samples * (bits_per_sample / 8);
    const uint32_t chunk_size = 36 + subchunk2_size;

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    /* RIFF */
    outfile.write("RIFF", 4);
    outfile.write(reinterpret_cast<const char *>(&chunk_size), 4);
    outfile.write("WAVE", 4);

    /* FORMAT */
    outfile.write("fmt ", 4);
    const uint32_t subchunk1_size = 16;
    outfile.write(reinterpret_cast<const char *>(&subchunk1_size), 4);
    outfile.write(reinterpret_cast<const char *>(&audio_format), 2);
    outfile.write(reinterpret_cast<const char *>(&num_channels), 2);
    outfile.write(reinterpret_cast<const char *>(&sample_rate_value), 4);
    outfile.write(reinterpret_cast<const char *>(&byte_rate), 4);
    outfile.write(reinterpret_cast<const char *>(&block_align), 2);
    outfile.write(reinterpret_cast<const char *>(&bits_per_sample), 2);

    /* DATA */
    outfile.write("data", 4);
    outfile.write(reinterpret_cast<const char *>(&subchunk2_size), 4);

    for (size_t i = 0; i < samples.size(); ++i) {
        if (samples[i].size() != num_channels) {
            outfile.close();
            throw std::runtime_error("Inconsistent number of channels in sample data at time step " + std::to_string(i));
        }
        outfile.write(reinterpret_cast<const char *>(samples[i].data()), num_channels * sizeof(float));
    }

    outfile.close();
}
