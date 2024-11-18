#include <array>
#include <iostream>
#include "song.hpp"
#include "driver/file.hpp"
#include "driver/port.hpp"
#include "gui/gui.hpp"

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
    std::cout << "Sample rate: " << sample_rate << std::endl;

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

    GUI gui;
    if (!gui.initialize()) {
        return 1;
    }

    port_audio_driver.play();
    while (!gui.is_done()) {
        gui.render();
    }

    gui.terminate();

#endif

    return 0;
}
