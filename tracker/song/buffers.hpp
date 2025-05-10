#ifndef SONG_BUFFERS_HPP
#define SONG_BUFFERS_HPP

#include <array>
#include <cstdint>
#include <string>
#include <utility>

#include "../constants.hpp"

struct Buffers {
    Buffers();
    void clear();

    std::array<std::array<uint8_t, MAX_STEPS>, MAX_SEQUENCES> sequences;
    std::array<std::array<std::pair<std::string, std::string>, MAX_STEPS>, MAX_COMMANDS_SEQUENCES> commands_sequences;
    std::array<std::array<uint8_t, MAX_ORDER_ITEMS>, MAX_ORDERS> orders;
    std::array<std::array<uint8_t, MAX_WAVETABLE_POINTS>, MAX_WAVETABLES> wavetables;
};

#endif // SONG_BUFFERS_HPP
