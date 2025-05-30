#include <iostream>

#include "../song/core.hpp"
#include "../song/functions.hpp"
#include "engine.hpp"

AudioEngine::AudioEngine(PortAudioDriver &driver)
    : driver(driver), playing(false), paused(false) {
    std::cout << "Starting playback with " << sample_rate << " Hz" << std::endl;
}

AudioEngine::~AudioEngine() {
    stop();
    join_thread();
    driver.close_stream();
}

void AudioEngine::play() {
    if (!driver.initialize()) {
        std::cerr << "Failed to initialize PortAudio." << std::endl;
        return;
    }
    if (playing) {
        paused = false;
        return;
    }
    if (!driver.open_stream()) {
        std::cerr << "Failed to open stream." << std::endl;
        return;
    }
    if (!driver.start_stream()) {
        std::cerr << "Failed to start stream." << std::endl;
        driver.close_stream();
        return;
    }

    initialize();
    playing = true;
    paused = false;
    join_thread();
    playback_thread = std::thread(&AudioEngine::playback_function, this);
}

void AudioEngine::pause() {
    paused = true;
}

void AudioEngine::stop() {
    playing = false;
    driver.buffer_cv.notify_all();
    join_thread();
    driver.stop_stream();
    driver.close_stream();
    driver.reset_buffer();
}

bool AudioEngine::is_playing() const {
    return playing && !paused;
}

bool AudioEngine::is_paused() const {
    return playing && paused;
}

void AudioEngine::playback_function() {
    size_t output_channels = driver.get_output_channels();
    unsigned long frames_per_half_buffer = driver.get_frames_per_buffer();
    unsigned long samples_per_half_buffer = frames_per_half_buffer * output_channels;

    while (playing) {
        std::unique_lock<std::mutex> lock(driver.buffer_mutex);
        driver.buffer_cv.wait(lock, [&] {
            return !playing || driver.half_consumed[0].load() || driver.half_consumed[1].load();
        });

        if (!playing) {
            break;
        }

        for (int phase = 0; phase < 2; ++phase) {
            if (driver.half_consumed[phase].load()) {
                unsigned long sample_offset = phase * samples_per_half_buffer;

                if (paused) {
                    std::fill_n(driver.pingpong_buffer.begin() + sample_offset, samples_per_half_buffer, 0.0f);
                } else {
                    for (unsigned long x = 0; x < frames_per_half_buffer; ++x) {
                        frame();
                        size_t offset = sample_offset + x * output_channels;
                        for (size_t i = 0; i < output_channels; ++i) {
                            driver.pingpong_buffer[offset + i] = output[i];
                        }
                    }
                }
                driver.half_consumed[phase].store(false);
            }
        }
    }
}

void AudioEngine::join_thread() {
    if (playback_thread.joinable()) {
        playback_thread.join();
    }
}

void AudioEngine::set_output_channels(const int channels) {
    driver.set_output_channels(channels);
}
