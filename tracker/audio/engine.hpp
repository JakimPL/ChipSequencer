#ifndef AUDIO_ENGINE_HPP
#define AUDIO_ENGINE_HPP

#define BUFFER_SIZE 512

#include <atomic>
#include <thread>
#include <vector>

#include "../constants.hpp"
#include "../general.hpp"
#include "../driver/port.hpp"
#include "../song/data.hpp"

class AudioEngine {
  public:
    AudioEngine(PortAudioDriver &driver);
    ~AudioEngine();

    void play();
    void pause();
    void stop();
    bool is_playing() const;

  private:
    void playback_function();

    PortAudioDriver &driver;
    std::atomic<bool> playing;
    std::atomic<bool> paused;
    std::thread playback_thread;

    static constexpr size_t buffer_size = BUFFER_SIZE;
};

#endif // AUDIO_ENGINE_HPP
