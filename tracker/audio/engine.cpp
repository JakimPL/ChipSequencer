#include <iostream>

#include "../song/data.hpp"
#include "../song/functions.hpp"
#include "engine.hpp"

AudioEngine::AudioEngine(PortAudioDriver &driver)
    : driver(driver), playing(false), paused(false) {
    std::cout << "Starting playback with " << sample_rate << " Hz" << std::endl;
}

AudioEngine::~AudioEngine() {
    stop();
    if (playback_thread.joinable()) {
        playback_thread.join();
    }
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
    playback_thread = std::thread(&AudioEngine::playback_function, this);
}

void AudioEngine::pause() {
    paused = true;
}

void AudioEngine::stop() {
    playing = false;
    driver.buffer_cv.notify_all();
    if (playback_thread.joinable()) {
        playback_thread.join();
    }
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
    while (playing) {
        std::unique_lock<std::mutex> lock(driver.buffer_mutex);
        driver.buffer_cv.wait(lock, [&] {
            return !playing || driver.half_consumed[0].load() || driver.half_consumed[1].load();
        });

        if (!playing) {
            break;
        }

        if (paused) {
            for (int phase = 0; phase < 2; ++phase) {
                if (driver.half_consumed[phase].load()) {
                    unsigned long offset = phase * driver.get_frames_per_buffer();
                    for (unsigned long i = 0; i < driver.get_frames_per_buffer(); ++i) {
                        driver.pingpong_buffer[offset + i] = 0;
                    }
                    driver.half_consumed[phase].store(false);
                }
            }
        } else {
            for (int phase = 0; phase < 2; ++phase) {
                if (driver.half_consumed[phase].load()) {
                    unsigned long offset = phase * driver.get_frames_per_buffer();
                    for (unsigned long i = 0; i < driver.get_frames_per_buffer(); ++i) {
                        mix();
                        driver.pingpong_buffer[offset + i] = output;
                    }
                    driver.half_consumed[phase].store(false);
                }
            }
        }
        lock.unlock();
    }
}
