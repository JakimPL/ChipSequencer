#ifndef MAPS_KEYS_HPP
#define MAPS_KEYS_HPP

#include "../gui/init.hpp"

struct KeyNoteMapping {
    ImGuiKey key;
    int note_index;
};

static KeyNoteMapping key_note_12_edo_mapping[] = {
    {ImGuiKey_Z, -9}, // C-4
    {ImGuiKey_S, -8}, // C#4
    {ImGuiKey_X, -7}, // D-4
    {ImGuiKey_D, -6}, // D#4
    {ImGuiKey_C, -5}, // E-4
    {ImGuiKey_V, -4}, // F-4
    {ImGuiKey_G, -3}, // F#4
    {ImGuiKey_B, -2}, // G-4
    {ImGuiKey_H, -1}, // G#4
    {ImGuiKey_N, 0},  // A-4
    {ImGuiKey_J, 1},  // A#4
    {ImGuiKey_M, 2},  // B-4

    {ImGuiKey_Comma, 3},     // C-5
    {ImGuiKey_L, 4},         // C#5
    {ImGuiKey_Period, 5},    // D-5
    {ImGuiKey_Semicolon, 6}, // D#5
    {ImGuiKey_Slash, 7},     // E-5

    {ImGuiKey_Q, 3},  // C-5
    {ImGuiKey_2, 4},  // C#5
    {ImGuiKey_W, 5},  // D-5
    {ImGuiKey_3, 6},  // D#5
    {ImGuiKey_E, 7},  // E-5
    {ImGuiKey_R, 8},  // F-5
    {ImGuiKey_5, 9},  // F#5
    {ImGuiKey_T, 10}, // G-5
    {ImGuiKey_6, 11}, // G#5
    {ImGuiKey_Y, 12}, // A-5
    {ImGuiKey_7, 13}, // A#5
    {ImGuiKey_U, 14}, // B-5

    {ImGuiKey_I, 15}, // C-6
    {ImGuiKey_9, 16}, // C#6
    {ImGuiKey_O, 17}, // D-6
    {ImGuiKey_0, 18}, // D#6
    {ImGuiKey_P, 19}  // E-6
};

static KeyNoteMapping key_note_linear_mapping[] = {
    {ImGuiKey_Z, -10},     // B-3
    {ImGuiKey_X, -9},      // C-4
    {ImGuiKey_C, -8},      // C#4
    {ImGuiKey_V, -7},      // D-4
    {ImGuiKey_B, -6},      // D#4
    {ImGuiKey_N, -5},      // E-4
    {ImGuiKey_M, -4},      // F-4
    {ImGuiKey_Comma, -3},  // F#4
    {ImGuiKey_Period, -2}, // G-4
    {ImGuiKey_Slash, -1},  // G#4

    {ImGuiKey_A, 0},           // A-4
    {ImGuiKey_S, 1},           // A#4
    {ImGuiKey_D, 2},           // B-4
    {ImGuiKey_F, 3},           // C-5
    {ImGuiKey_G, 4},           // C#5
    {ImGuiKey_H, 5},           // D-5
    {ImGuiKey_J, 6},           // D#5
    {ImGuiKey_K, 7},           // E-5
    {ImGuiKey_L, 8},           // F-5
    {ImGuiKey_Semicolon, 9},   // F#5
    {ImGuiKey_Apostrophe, 10}, // G-5

    {ImGuiKey_Q, 11},           // G#5
    {ImGuiKey_W, 12},           // A-5
    {ImGuiKey_E, 13},           // A#5
    {ImGuiKey_R, 14},           // B-5
    {ImGuiKey_T, 15},           // C-6
    {ImGuiKey_Y, 16},           // C#6
    {ImGuiKey_U, 17},           // D-6
    {ImGuiKey_I, 18},           // D#6
    {ImGuiKey_O, 19},           // E-6
    {ImGuiKey_P, 20},           // F-6
    {ImGuiKey_LeftBracket, 21}, // F#6
    {ImGuiKey_RightBracket, 22} // G-6
};

constexpr std::array<ImGuiKey, 7> commands_keys = {
    ImGuiKey_U,
    ImGuiKey_D,
    ImGuiKey_B,
    ImGuiKey_S,
    ImGuiKey_G,
    ImGuiKey_M,
    ImGuiKey_A,
};

constexpr std::array<ImGuiKey, 12> values_keys = {
    ImGuiKey_0,
    ImGuiKey_1,
    ImGuiKey_2,
    ImGuiKey_3,
    ImGuiKey_4,
    ImGuiKey_5,
    ImGuiKey_6,
    ImGuiKey_7,
    ImGuiKey_8,
    ImGuiKey_9,
    ImGuiKey_Period,
    ImGuiKey_Comma,
};

#endif // MAPS_KEYS_HPP
