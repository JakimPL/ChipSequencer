#include "buffers.hpp"

Buffers::Buffers() {
    clear();
}

void Buffers::clear() {
    for (size_t i = 0; i < MAX_SEQUENCES; ++i) {
        sequences[i].fill(NOTE_REST);
    }

    for (size_t i = 0; i < MAX_COMMANDS_SEQUENCES; ++i) {
        for (size_t j = 0; j < MAX_STEPS; ++j) {
            commands_sequences[i][j] = {"", ""};
        }
    }

    for (size_t i = 0; i < MAX_ORDERS; ++i) {
        orders[i].fill(0);
    }

    for (size_t i = 0; i < MAX_WAVETABLES; ++i) {
        wavetables[i].fill(0x80);
    }
}
