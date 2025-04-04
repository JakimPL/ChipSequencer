#include "general.hpp"
#include "song/data.hpp"

ScaleComposer scale_composer = ScaleComposer();
FrequencyTable frequency_table = FrequencyTable(scale_composer, DEFAULT_A4_FREQUENCY);

Song song = {
    bpm,
    normalizer,
    num_channels,
    num_dsps,
    reference_frequency,
    note_divisor,
    envelopes,
    sequences,
    orders,
    oscillators,
    wavetables,
    dsps,
    channels,
    buffer_offsets,
    links,
    scale_composer,
    frequency_table,
};

GUI gui = GUI();
std::filesystem::path current_path;

void terminate() {
    gui.terminate();
    SDL_Quit();
    exit(0);
}
