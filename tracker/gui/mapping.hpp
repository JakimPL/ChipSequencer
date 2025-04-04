#ifndef GUI_MAPPING_HPP
#define GUI_MAPPING_HPP

#include "init.hpp"

struct KeyNoteMapping {
    ImGuiKey key;
    int note_index;
};

static KeyNoteMapping key_note_12_edo_mappings[] = {
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

#endif // GUI_MAPPING_HPP
