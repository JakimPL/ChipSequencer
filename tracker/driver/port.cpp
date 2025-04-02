#include "port.hpp"
#include <iostream>
#include <portaudio.h>

PortAudioDriver::PortAudioDriver(
    const std::array<t_output, SONG_LENGTH> &target,
    uint16_t sample_rate,
    unsigned long frames_per_buffer
)
    : Driver(target),
      sample_rate(sample_rate),
      stream(nullptr),
      current_index(0),
      frames_per_buffer(frames_per_buffer),
      write_index(0),
      read_index(0),
      target(target) {
    circ_buffer_size = CIRC_BUFFER_CAPACITY;
    circ_buffer.resize(circ_buffer_size, 0);
}

void PortAudioDriver::submit_buffer(const t_output *data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        size_t current_write = write_index.load(std::memory_order_relaxed);
        size_t next_write = (current_write + 1) % circ_buffer_size;
        if (next_write == read_index.load(std::memory_order_acquire)) {
            /* Buffer full; drop sample */
            break;
        }
        circ_buffer[current_write] = data[i];
        write_index.store(next_write, std::memory_order_release);
    }
}

void PortAudioDriver::play() {
    current_index = 0;
    if (!open_stream()) {
        return;
    }
    if (!start_stream()) {
        close_stream();
    }
    sleep();
    stop_stream();
    close_stream();
}

void PortAudioDriver::terminate() {
    Pa_Terminate();
}

bool PortAudioDriver::initialize() {
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        return false;
    }
    return true;
}

bool PortAudioDriver::open_stream() {
    PaError err = Pa_OpenDefaultStream(
        &stream,
        0,
        1,
        paFloat32,
        sample_rate,
        frames_per_buffer,
        audio_callback,
        this
    );
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return false;
    }
    return true;
}

bool PortAudioDriver::close_stream() {
    PaError err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        Pa_Terminate();
        return false;
    }
    return true;
}

bool PortAudioDriver::start_stream() {
    PaError err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return false;
    }
    return true;
}

bool PortAudioDriver::stop_stream() {
    PaError err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
        Pa_CloseStream(stream);
        Pa_Terminate();
        return false;
    }
    return true;
}

void PortAudioDriver::sleep() {
    Pa_Sleep(SONG_LENGTH * 1000 / sample_rate);
}

int PortAudioDriver::audio_callback(
    const void *input_buffer,
    void *output_buffer,
    unsigned long frames_per_buffer,
    const PaStreamCallbackTimeInfo *time_info,
    PaStreamCallbackFlags status_flags,
    void *user_data
) {
    auto *driver = static_cast<PortAudioDriver *>(user_data);
    float *out = reinterpret_cast<float *>(output_buffer);
    (void) input_buffer;
    for (unsigned long i = 0; i < frames_per_buffer; ++i) {
        size_t current_read = driver->read_index.load(std::memory_order_relaxed);
        if (current_read != driver->write_index.load(std::memory_order_acquire)) {
            *out++ = driver->circ_buffer[current_read];
            size_t next_read = (current_read + 1) % driver->circ_buffer_size;
            driver->read_index.store(next_read, std::memory_order_release);
        } else {
            *out++ = 0.0f;
        }
    }
    return paContinue;
}
