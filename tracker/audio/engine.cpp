#include <cstring>
#include <iostream>
#include <signal.h>
#include <setjmp.h>

#include "../song/core.hpp"
#include "../song/functions.hpp"
#include "engine.hpp"

static thread_local sigjmp_buf jump_buffer;
static thread_local bool segfault_occurred = false;
static void segfault_handler(int signal) {
    segfault_occurred = true;
    siglongjmp(jump_buffer, 1);
}

AudioEngine::AudioEngine(PortAudioDriver &driver)
    : driver(driver), playing(false), paused(false) {
    std::cout << "Starting playback with " << sample_rate << " Hz" << std::endl;
    for (auto &channel_history : history) {
        channel_history.resize(HISTORY_SIZE, 0.0f);
    }
}

AudioEngine::~AudioEngine() {
    stop();
    join_thread();
    driver.close_stream();
}

void AudioEngine::play(const uint16_t from_row) {
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

    playing = true;
    paused = false;
    error = false;
    initialize();
    skip_rows(from_row);
    join_thread();
    playback_thread = std::thread(&AudioEngine::playback_function, this);
}

void AudioEngine::skip_rows(const uint16_t from_row) {
    if (from_row == 0) {
        return;
    }

    while (global_row <= from_row) {
        frame();
    }
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

bool AudioEngine::is_error() {
    if (error) {
        error = false;
        return true;
    }

    return false;
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

    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = segfault_handler;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);

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
                        segfault_occurred = false;
                        if (sigsetjmp(jump_buffer, 1) == 0) {
                            frame();
                        } else {
                            std::cerr << "Recovered from segfault in audio processing" << std::endl;
                            playing = false;
                            error = true;
                            driver.stop_stream();
                            driver.close_stream();
                            break;
                        }

                        size_t offset = sample_offset + x * output_channels;
                        for (size_t i = 0; i < output_channels; ++i) {
                            driver.pingpong_buffer[offset + i] = output[i];
                            std::lock_guard<std::mutex> lock(history_mutex);
                            history[i].push_back(output[i]);
                            if (history[i].size() > buffer_size) {
                                history[i].pop_front();
                            }
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

const AudioHistory &AudioEngine::get_history() const {
    return history;
}

void AudioEngine::clear_history() {
    for (auto &channel_history : history) {
        channel_history.clear();
        channel_history.resize(HISTORY_SIZE, 0.0f);
    }
}

void AudioEngine::lock_history() const {
    history_mutex.lock();
}

void AudioEngine::unlock_history() const {
    history_mutex.unlock();
}
