#pragma once

#include <string>
#include "driver.hpp"

class FileDriver : public Driver {
  public:
    void terminate() override;
    void play() override;

    void set_length(size_t length);
    void set_sample_rate(int rate);
    void set_output_channels(int channels);
    void set_output_filename(const std::string &filename);
    void set(const std::string &filename, int rate, int channels, size_t length);

  private:
    void save_output_to_file();
    std::vector<std::vector<float>> samples;

    std::string filename;
    size_t song_length;
    int sample_rate;
    int output_channels;
};
