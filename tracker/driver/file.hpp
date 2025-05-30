#ifndef DRIVER_FILE_HPP
#define DRIVER_FILE_HPP

#include <string>
#include "driver.hpp"

class FileDriver : public Driver {
  public:
    FileDriver();

    void terminate() override;
    void play() override;

    void set_length(const size_t length);
    void set_sample_rate(const int rate);
    void set_output_channels(const int channels);
    void set_output_filename(const std::string &filename);
    void set(const std::string &filename, const int rate, const int channels, const size_t length);

  private:
    void save_output_to_file();
    std::vector<std::vector<float>> samples;

    std::string filename;
    size_t song_length;
    int sample_rate;
    int output_channels;
};

#endif // DRIVER_FILE_HPP