#ifndef DRIVER_PORT_HPP
#define DRIVER_PORT_HPP

#include <iostream>
#include <portaudio.h>
#include "driver.hpp"

class PortAudioDriver : public Driver {
  public:
    PortAudioDriver(
        const std::array<t_output, SONG_LENGTH> &target,
        uint16_t sample_rate,
        unsigned long frames_per_buffer = 256
    );

    void play() override;
    void terminate() override;
    bool initialize() override;

    bool open_stream();
    bool close_stream();
    bool start_stream();
    bool stop_stream();
    void sleep();

  private:
    static int audio_callback(
        const void *input_buffer,
        void *output_buffer,
        unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags,
        void *user_data
    );

    uint16_t sample_rate;
    PaStream *stream;
    uint current_index;
    unsigned long frames_per_buffer;
};

#endif // DRIVER_PORT_HPP