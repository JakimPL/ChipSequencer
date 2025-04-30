#include "buffers.hpp"

Buffers::Buffers() {
    clear();
}

void Buffers::clear() {
    for (size_t i = 0; i < MAX_SEQUENCES; ++i) {
        sequences[i].fill(NOTE_REST);
    }

    for (size_t i = 0; i < MAX_WAVETABLES; ++i) {
        wavetables[i].fill(0x80);
    }
}
