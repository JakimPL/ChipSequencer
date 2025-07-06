#pragma once

#include <cstdint>
#include <optional>
#include <vector>

#include "../../structures/sequence.hpp"

struct Pattern {
    Pattern() = default;
    explicit Pattern(uint8_t sequence_index);

    bool bypass = false;
    size_t id = 0;
    int starting_row = 0;
    int current_row = -1;
    uint8_t sequence_index = -1;
    int steps = 0;
    std::vector<uint8_t> notes = {};
    std::vector<uint8_t> durations = {};
    std::vector<uint8_t> indices = {};

    void from_sequence(uint8_t sequence_index);
    std::vector<Note> to_note_vector() const;
    void to_buffer(size_t sequence_index) const;
    int calculate_playing_row(size_t channel_index);

    void clear_row(int row);
    bool is_row_valid(int row) const;
    void jump(int max_row = -1);
    void insert_note(int note_index, int edo, int max_row = -1);
    int get_note(int row) const;
    void set_note(int row, int note);
    void transpose(int value, std::optional<int> row = std::nullopt);
    void handle_input(int min_row = 0, int max_row = -1);
};
