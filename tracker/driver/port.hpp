#ifndef DRIVER_PORT_HPP
#define DRIVER_PORT_HPP

#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <portaudio.h>
#include "driver.hpp"

#define DEFAULT_FRAMES_PER_BUFFER 1024

class PortAudioDriver : public Driver {
  public:
    PortAudioDriver(
        unsigned long frames_per_buffer = DEFAULT_FRAMES_PER_BUFFER
    );

    void play() override;
    void terminate() override;
    bool initialize() override;

    bool open_stream();
    bool close_stream();
    bool start_stream();
    bool stop_stream();

    void reset_buffer();
    void submit_buffer(const _Float32 *data, size_t size);

    std::vector<_Float32> pingpong_buffer;
    unsigned long get_frames_per_buffer() const { return frames_per_buffer; }

    std::mutex buffer_mutex;
    std::condition_variable buffer_cv;
    std::atomic<bool> half_consumed[2];
    std::atomic<int> current_phase;

  private:
    uint16_t rate;
    PaStream *stream;
    uint current_index;
    unsigned long frames_per_buffer;

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
