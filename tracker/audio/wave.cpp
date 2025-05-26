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
