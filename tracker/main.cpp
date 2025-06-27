#include <iostream>
#include <stdexcept>
#include <string>

#include "general.hpp"
#include "audio/engine.hpp"
#include "driver/file.hpp"
#include "driver/port.hpp"
#include "gui/gui.hpp"
#include "song/functions.hpp"

int main(int argc, char *argv[]) {
    if (!gui.initialize()) {
        return 1;
    }

    PortAudioDriver port_audio_driver = PortAudioDriver();
    port_audio_driver.initialize();
    AudioEngine audio_engine(port_audio_driver);
    gui.set_audio_engine(&audio_engine);

    if (argc > 1) {
        std::string filename = argv[1];
        try {
            gui.open(filename);
        } catch (std::runtime_error &exception) {
            std::cerr << "Failed to read data file: " << exception.what() << std::endl;
        }
    }

    while (!gui.is_done()) {
        gui.render();
    }

    terminate();

    return 0;
}
