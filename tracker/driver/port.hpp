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
        unsigned long frames_per_buffer = 256)
        : Driver(target), sample_rate(sample_rate), stream(nullptr), current_index(0), frames_per_buffer(frames_per_buffer) {}

    bool initialize() override {
        PaError err = Pa_Initialize();
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            return false;
        }
        return true;
    }

    bool open_stream() {
        PaError err = Pa_OpenDefaultStream(
            &stream,
            0,
            1,
            paFloat32,
            sample_rate,
            256,
            audio_callback,
            this);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            Pa_Terminate();
            return false;
        }
        return true;
    }

    bool close_stream() {
        PaError err = Pa_CloseStream(stream);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            Pa_Terminate();
            return false;
        }
        return true;
    }

    bool start_stream() {
        PaError err = Pa_StartStream(stream);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            Pa_CloseStream(stream);
            Pa_Terminate();
            return false;
        }
        return true;
    }

    bool stop_stream() {
        PaError err = Pa_StopStream(stream);
        if (err != paNoError) {
            std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
            Pa_CloseStream(stream);
            Pa_Terminate();
            return false;
        }
        return true;
    }

    void sleep() {
        Pa_Sleep(SONG_LENGTH * 1000 / sample_rate);
    }

    void play() override {
        if (!open_stream()) {
            return;
        }

        if (!start_stream()) {
            close_stream();
        }

        sleep();
        stop_stream();
        close_stream();

        Pa_Terminate();
    }

  private:
    static int audio_callback(
        const void *input_buffer,
        void *output_buffer,
        unsigned long frames_per_buffer,
        const PaStreamCallbackTimeInfo *time_info,
        PaStreamCallbackFlags status_flags,
        void *user_data) {
        auto *driver = static_cast<PortAudioDriver *>(user_data);
        float *out = (float *) output_buffer;
        (void) input_buffer;

        for (unsigned long i = 0; i < frames_per_buffer; ++i) {
            if (driver->current_index < SONG_LENGTH) {
                *out++ = driver->target[driver->current_index++];
            } else {
                *out++ = 0.0f;
            }
        }

        return paContinue;
    }

    uint16_t sample_rate;
    PaStream *stream;
    uint current_index;
    unsigned long frames_per_buffer;
};

#endif // DRIVER_PORT_HPP
