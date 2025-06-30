#ifdef _WIN32
#define SDL_MAIN_HANDLED
#endif

#include <iostream>
#include <stdexcept>
#include <string>

#include "general.hpp"
#include "audio/engine.hpp"
#include "driver/file.hpp"
#include "driver/port.hpp"
#include "gui/gui.hpp"
#include "song/functions.hpp"

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int argc;
    LPWSTR *argv_w = CommandLineToArgvW(GetCommandLineW(), &argc);

    char **argv = new char *[argc];
    for (int i = 0; i < argc; i++) {
        int len = WideCharToMultiByte(CP_UTF8, 0, argv_w[i], -1, NULL, 0, NULL, NULL);
        argv[i] = new char[len];
        WideCharToMultiByte(CP_UTF8, 0, argv_w[i], -1, argv[i], len, NULL, NULL);
    }
    LocalFree(argv_w);
#else
int main(int argc, char *argv[]) {
#endif

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

#ifdef _WIN32
    for (int i = 0; i < argc; i++) {
        delete[] argv[i];
    }
    delete[] argv;
#endif

    return 0;
}
