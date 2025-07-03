#pragma once

#include <atomic>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

#include "../driver/port.hpp"

constexpr int BUFFER_SIZE = 128;
constexpr int HISTORY_SIZE = 65536;

using AudioHistory = std::array<std::deque<_Float32>, MAX_OUTPUT_CHANNELS>;

class AudioEngine {
  public:
    explicit AudioEngine(PortAudioDriver &driver);
    ~AudioEngine();

    // Delete copy/move operations - this class manages threads and has reference members
    AudioEngine(const AudioEngine &) = delete;
    AudioEngine &operator=(const AudioEngine &) = delete;
    AudioEngine(AudioEngine &&) = delete;
    AudioEngine &operator=(AudioEngine &&) = delete;

    void play(uint16_t from_row = 0);
    void pause();
    void stop();
    bool is_error();
    bool is_playing() const;
    bool is_paused() const;

    void skip_rows(uint16_t from_row = 0);
    void set_output_channels(int channels);

    const AudioHistory &get_history() const;
    void clear_history();
    void lock_history() const;
    void unlock_history() const;

  private:
    void playback_function();
    bool safe_frame();
    void handle_frame_exception();

    PortAudioDriver &driver;
    std::atomic<bool> error;
    std::atomic<bool> playing;
    std::atomic<bool> paused;
    std::thread playback_thread;

    std::array<std::deque<_Float32>, MAX_OUTPUT_CHANNELS> history;
    mutable std::mutex history_mutex;
    static constexpr size_t buffer_size = BUFFER_SIZE;

    void join_thread();
};
