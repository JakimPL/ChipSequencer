#ifndef AUDIO_WAVE_HPP
#define AUDIO_WAVE_HPP

#include <sndfile.h>
#include <stdexcept>
#include <string>
#include <vector>

void save_wave(
    const std::string &filename,
    const std::vector<std::vector<float>> &samples,
    int sample_rate,
    int output_channels
);

#endif // AUDIO_WAVE_HPP
