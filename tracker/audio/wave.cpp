#include <stdexcept>

#include "wave.hpp"

#define IEEE_FLOAT 3
#define BITS_PER_SAMPLE 32

void save_wave(const std::string &filename, const std::vector<std::vector<float>> &samples, int sample_rate, int output_channels) {
    SF_INFO sfinfo;
    sfinfo.channels = output_channels;
    sfinfo.samplerate = sample_rate;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
    sfinfo.frames = samples.size();
    sfinfo.sections = 0;
    sfinfo.seekable = 0;

    SNDFILE *sndfile = sf_open(filename.c_str(), SFM_WRITE, &sfinfo);
    if (!sndfile) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    for (const auto &frame : samples) {
        if (sf_writef_float(sndfile, frame.data(), 1) != 1) {
            sf_close(sndfile);
            throw std::runtime_error("Failed to write samples to file: " + filename);
        }
    }

    sf_close(sndfile);
}

void save_wave(const std::string &filename, const Samples &samples) {
    save_wave(filename, samples.data, samples.sample_rate, samples.output_channels);
}

Samples load_wave(const std::string &filename) {
    SF_INFO sfinfo;
    SNDFILE *sndfile = sf_open(filename.c_str(), SFM_READ, &sfinfo);
    if (!sndfile) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    const int sample_rate = sfinfo.samplerate;
    const int output_channels = sfinfo.channels;
    std::vector<std::vector<float>> samples(sfinfo.frames, std::vector<float>(sfinfo.channels));

    for (sf_count_t i = 0; i < sfinfo.frames; ++i) {
        if (sf_readf_float(sndfile, samples[i].data(), 1) != 1) {
            sf_close(sndfile);
            throw std::runtime_error("Failed to read samples from file: " + filename);
        }
    }

    sf_close(sndfile);
    return {samples, sample_rate, output_channels};
}
