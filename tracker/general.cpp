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

static void new_song() {
    song.new_song();
}
