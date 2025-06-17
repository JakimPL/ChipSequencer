#pragma once

#include <atomic>
#include <deque>
#include <thread>
#include <vector>

#include "../driver/port.hpp"

constexpr int BUFFER_SIZE = 512;
constexpr int HISTORY_SIZE = 96000;

class AudioEngine {
  public:
    AudioEngine(PortAudioDriver &driver);
    ~AudioEngine();

    void play();
    void pause();
    void stop();
    bool is_error();
    bool is_playing() const;
    bool is_paused() const;

    void set_output_channels(const int channels);
    const std::vector<std::deque<_Float32>> &get_history() const;

  private:
    void playback_function();

    PortAudioDriver &driver;
    std::atomic<bool> error;
    std::atomic<bool> playing;
    std::atomic<bool> paused;
    std::thread playback_thread;

    std::vector<std::deque<_Float32>> history;
    static constexpr size_t buffer_size = BUFFER_SIZE;

    void join_thread();
};
