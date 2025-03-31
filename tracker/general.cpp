#include "general.hpp"
#include "song/data.hpp"

Song song = {
    bpm,
    normalizer,
    envelopes,
    sequences,
    orders,
    oscillators,
    wavetables,
    dsps,
    channels,
    buffer_offsets,
    links
};

GUI gui = GUI();

void terminate() {
    gui.terminate();
    SDL_Quit();
    exit(0);
}
