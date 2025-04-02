#ifndef DRIVER_PORT_HPP
#define DRIVER_PORT_HPP

#include <array>
#include <vector>
#include <atomic>
#include <portaudio.h>
#include "driver.hpp"

#define CIRC_BUFFER_CAPACITY 4096

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

    void submit_buffer(const t_output *data, size_t size);

    friend int audio_callback(
        const void *input_buffer,
        void *output_buffer,
        unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags,
        void *user_data
    );

  private:
    uint16_t sample_rate;
    PaStream *stream;
    uint current_index;
    unsigned long frames_per_buffer;

    std::vector<t_output> circ_buffer;
    std::atomic<size_t> write_index;
    std::atomic<size_t> read_index;
    size_t circ_buffer_size;

    const std::array<t_output, SONG_LENGTH> &target;

    static int audio_callback(
        const void *input_buffer,
        void *output_buffer,
        unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags,
        void *user_data
    );
};

#endif // DRIVER_PORT_HPP