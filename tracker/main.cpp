#include <array>
#include <iostream>
#include "song/song.hpp"
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

void render(std::array<t_output, SONG_LENGTH> &target) {
    for (uint i = 0; i < SONG_LENGTH; ++i) {
        mix();
        target[i] = output;
    }
}

void play_audio(PortAudioDriver &port_audio_driver, std::array<t_output, SONG_LENGTH> &target, GUI &gui) {
    render(target);
    gui.set_playing_status(true);
    port_audio_driver.play();
    gui.set_playing_status(false);
}

int main() {
    std::cout << "Starting the program..." << std::endl;
    initialize();
    std::cout << "ChipSequencer initialized!" << std::endl;
    std::cout << "Sample rate: " << sample_rate << std::endl;

    std::array<t_output, SONG_LENGTH> target;
    render(target);

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

    gui.set_play_callback([&]() {
        std::thread audio_thread(play_audio, std::ref(port_audio_driver), std::ref(target), std::ref(gui));
        audio_thread.detach();
    });

    while (!gui.is_done()) {
        gui.render();
    }

    gui.terminate();

    Song song = {
        bpm,
        normalizer,
        CHANNEL_SIZE,
        SONG_LENGTH,
        envelopes,
        sequences,
        orders,
        oscillators,
        wavetables,
        dsps,
        channels,
        links
    };

    song.save_to_file("song.seq");

#endif

    return 0;
}
