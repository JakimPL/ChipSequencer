#include <array>
#include <iostream>
#include "song.hpp"
#include "driver/file.hpp"
#include "driver/port.hpp"

extern "C" {
void sound_driver_initialize() __attribute__((used));
void sound_driver_terminate() __attribute__((used));
void sound_driver_step() __attribute__((used));
void initialize();
void mix();
}

void sound_driver_initialize() {
    return;
}

void sound_driver_terminate() {
    return;
}

void sound_driver_step() {
    return;
}

static std::array<t_output, SONG_LENGTH> target;
static uint32_t song_index = 0;

int main() {
    std::cout << "Starting the program..." << std::endl;
    initialize();
    std::cout << "ChipSequencer initialized!" << std::endl;
    std::cout << sample_rate << std::endl;

    // sample_rate = 22050;

    for (uint i = 0; i < SONG_LENGTH; ++i) {
        mix();
        target[i] = output;
    }

#if SAVE_TO_FILE
    FileDriver file_driver = FileDriver(target, "output.txt");
    file_driver.initialize();
    file_driver.play();
#else
    PortAudioDriver port_audio_driver = PortAudioDriver(target, sample_rate);
    port_audio_driver.initialize();
    port_audio_driver.play();
#endif

    return 0;
}
