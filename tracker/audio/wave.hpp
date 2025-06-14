#pragma once

#include <sndfile.h>
#include <stdexcept>
#include <string>
#include <vector>

struct Samples {
    std::vector<std::vector<float>> data;
    int sample_rate;
    int output_channels;
};

void save_wave(
    const std::string &filename,
    const Samples &samples
);

void save_wave(
    const std::string &filename,
    const std::vector<std::vector<float>> &samples,
    const int sample_rate,
    const int output_channels
);

Samples load_wave(const std::string &filename);
