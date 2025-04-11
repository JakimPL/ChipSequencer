#include <array>
#include <iostream>

#include "general.hpp"
#include "audio/engine.hpp"
#include "driver/file.hpp"
#include "driver/port.hpp"
#include "gui/gui.hpp"
#include "song/functions.hpp"

int main() {
    if (!gui.initialize()) {
        return 1;
    }

#if SAVE_TO_FILE
    FileDriver file_driver = FileDriver(target, "output.txt");
    file_driver.initialize();
    file_driver.play();
#else
    PortAudioDriver port_audio_driver = PortAudioDriver();
    port_audio_driver.initialize();
    AudioEngine audio_engine(port_audio_driver);
    gui.set_audio_engine(&audio_engine);

    while (!gui.is_done()) {
        gui.render();
    }

    terminate();

#endif
    return 0;
}
