#include <chrono>
#include <iostream>

#include "../song/functions.hpp"
#include "engine.hpp"

AudioEngine::AudioEngine(PortAudioDriver &driver)
    : driver(driver), playing(false), paused(false) {
}

AudioEngine::~AudioEngine() {
    stop();
    if (playback_thread.joinable()) {
        playback_thread.join();
    }
    driver.close_stream();
}

void AudioEngine::play() {
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
    playing = true;
    paused = false;
    playback_thread = std::thread(&AudioEngine::playback_function, this);
}

void AudioEngine::pause() {
    paused = true;
}

void AudioEngine::stop() {
    playing = false;
}

bool AudioEngine::is_playing() const {
    return playing;
}

void AudioEngine::playback_function() {
    std::vector<t_output> buffer(buffer_size);
    while (playing) {
        if (paused) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }
        for (size_t i = 0; i < buffer_size; ++i) {
            mix();
            buffer[i] = output;
        }
        driver.submit_buffer(buffer.data(), buffer_size);
    }
}