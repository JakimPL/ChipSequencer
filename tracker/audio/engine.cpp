#include <cstring>
#include <iostream>
#include <setjmp.h>

#ifdef _WIN32
#include <windows.h>
#include <excpt.h>
#else
#include <signal.h>
#endif

#include "../song/core.hpp"
#include "../song/functions.hpp"
#include "engine.hpp"

#ifdef CATCH_SEGFAULT
#ifdef _WIN32
static thread_local bool exception_occurred = false;
static thread_local jmp_buf jump_buffer_win;

#define SETUP_EXCEPTION_FILTER() \
    LPTOP_LEVEL_EXCEPTION_FILTER old_filter = SetUnhandledExceptionFilter(unhandled_exception_filter);

#define RESTORE_EXCEPTION_FILTER() \
    SetUnhandledExceptionFilter(old_filter);

static LONG WINAPI unhandled_exception_filter(EXCEPTION_POINTERS *ep) {
    DWORD code = ep->ExceptionRecord->ExceptionCode;
    if (code == EXCEPTION_ACCESS_VIOLATION ||
        code == EXCEPTION_ARRAY_BOUNDS_EXCEEDED ||
        code == EXCEPTION_DATATYPE_MISALIGNMENT ||
        code == EXCEPTION_FLT_DIVIDE_BY_ZERO ||
        code == EXCEPTION_FLT_OVERFLOW ||
        code == EXCEPTION_FLT_UNDERFLOW ||
        code == EXCEPTION_ILLEGAL_INSTRUCTION ||
        code == EXCEPTION_IN_PAGE_ERROR ||
        code == EXCEPTION_INT_DIVIDE_BY_ZERO ||
        code == EXCEPTION_INT_OVERFLOW ||
        code == EXCEPTION_PRIV_INSTRUCTION ||
        code == EXCEPTION_STACK_OVERFLOW) {
        exception_occurred = true;
        longjmp(jump_buffer_win, 1);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
#else
static thread_local sigjmp_buf jump_buffer;
static thread_local bool segfault_occurred = false;

#define SETUP_EXCEPTION_FILTER()              \
    struct sigaction sa;                      \
    memset(&sa, 0, sizeof(struct sigaction)); \
    sa.sa_handler = segfault_handler;         \
    sigemptyset(&sa.sa_mask);                 \
    sigaction(SIGSEGV, &sa, NULL);

#define RESTORE_EXCEPTION_FILTER()

static void segfault_handler(int signal) {
    segfault_occurred = true;
    siglongjmp(jump_buffer, 1);
}
#endif
#else
#define SETUP_EXCEPTION_FILTER()
#define RESTORE_EXCEPTION_FILTER()
#endif

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

bool AudioEngine::safe_frame() {
#ifdef CATCH_SEGFAULT
#ifdef _WIN32
    exception_occurred = false;
    if (setjmp(jump_buffer_win) == 0) {
        frame();
        return true;
    } else {
        return false;
    }
#else
    segfault_occurred = false;
    if (sigsetjmp(jump_buffer, 1) == 0) {
        frame();
        return true;
    } else {
        return false;
    }
#endif
#else
    frame();
    return true;
#endif
}

void AudioEngine::handle_frame_exception() {
    std::cerr << "Recovered from exception in audio processing" << std::endl;
    playing = false;
    error = true;
    driver.stop_stream();
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
    initialize();
    skip_rows(from_row);
    join_thread();
    playback_thread = std::thread(&AudioEngine::playback_function, this);
}

void AudioEngine::skip_rows(const uint16_t from_row) {
    if (from_row == 0) {
        return;
    }

    SETUP_EXCEPTION_FILTER()

    while (global_row <= from_row) {
        if (!safe_frame()) {
            handle_frame_exception();
            break;
        }
    }

    RESTORE_EXCEPTION_FILTER()
}

void AudioEngine::pause() {
    paused = true;
}

void AudioEngine::stop() {
    if (!playing) {
        return;
    }

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

    SETUP_EXCEPTION_FILTER()

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
                        if (!safe_frame()) {
                            handle_frame_exception();
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

    RESTORE_EXCEPTION_FILTER()
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
