#include <array>
#include <iostream>

#include "general.hpp"
#include "audio/engine.hpp"
#include "driver/file.hpp"
#include "driver/port.hpp"
#include "gui/gui.hpp"
#include "song/functions.hpp"

int main() {
    std::cout << "Starting the program..." << std::endl;
    initialize();
    std::cout << "ChipSequencer initialized!" << std::endl;
    std::cout << "Sample rate: " << sample_rate << std::endl;

    if (!gui.initialize()) {
        return 1;
    }

    std::array<t_output, SONG_LENGTH> target;

#if SAVE_TO_FILE
    FileDriver file_driver = FileDriver(target, "output.txt");
    file_driver.initialize();
    file_driver.play();
#else
    PortAudioDriver port_audio_driver = PortAudioDriver(target, sample_rate);
    port_audio_driver.initialize();
    AudioEngine audio_engine(port_audio_driver);

    gui.set_play_callback([&]() {
        audio_engine.play();
    });
    gui.set_pause_callback([&]() {
        audio_engine.pause();
    });
    gui.set_stop_callback([&]() {
        audio_engine.stop();
    });

    while (!gui.is_done()) {
        gui.render();
    }

    terminate();

#endif
    return 0;
}
