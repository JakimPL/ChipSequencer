#ifndef AUDIO_ENGINE_HPP
#define AUDIO_ENGINE_HPP

#define BUFFER_SIZE 512

#include <atomic>
#include <thread>
#include <vector>

#include "../driver/port.hpp"

class AudioEngine {
  public:
    AudioEngine(PortAudioDriver &driver);
    ~AudioEngine();

    void play();
    void pause();
    void stop();
    bool is_playing() const;
    bool is_paused() const;

  private:
    void playback_function();

    PortAudioDriver &driver;
    std::atomic<bool> playing;
    std::atomic<bool> paused;
    std::thread playback_thread;

    static constexpr size_t buffer_size = BUFFER_SIZE;
};

#endif // AUDIO_ENGINE_HPP
