#include <algorithm>
#include <iostream>
#include <mutex>
#include <portaudio.h>

#include "port.hpp"

PortAudioDriver::PortAudioDriver(
    unsigned long frames_per_buffer
)
    : Driver(),
      stream(nullptr),
      current_index(0),
      frames_per_buffer(frames_per_buffer) {
    pingpong_buffer.resize(2 * frames_per_buffer, 0);
    half_consumed[0].store(true);
    half_consumed[1].store(true);
    current_phase.store(0);
}

void PortAudioDriver::reset_buffer() {
    std::lock_guard<std::mutex> lock(buffer_mutex);
    std::fill(pingpong_buffer.begin(), pingpong_buffer.end(), 0);
    half_consumed[0].store(true);
    half_consumed[1].store(true);
    current_phase.store(0);
    buffer_cv.notify_all();
}

void PortAudioDriver::submit_buffer(const _Float32 *data, size_t size) {
    std::unique_lock<std::mutex> lock(buffer_mutex);
    if (size > pingpong_buffer.size()) {
        size = pingpong_buffer.size();
    }
    std::copy(data, data + size, pingpong_buffer.begin());
    lock.unlock();
    buffer_cv.notify_one();
}

void PortAudioDriver::play() {
    current_index = 0;
    if (!open_stream()) {
        return;
    }
    if (!start_stream()) {
        close_stream();
        return;
    }
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
    int phase = driver->current_phase.load();
    unsigned long offset = phase * driver->frames_per_buffer;
    for (unsigned long i = 0; i < driver->frames_per_buffer; ++i) {
        *out++ = driver->pingpong_buffer[offset + i];
    }
    driver->half_consumed[phase].store(true);
    driver->buffer_cv.notify_one();
    driver->current_phase.store(1 - phase);
    return paContinue;
}