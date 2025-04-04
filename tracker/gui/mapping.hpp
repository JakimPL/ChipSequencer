#ifndef GUI_MAPPING_HPP
#define GUI_MAPPING_HPP

#include "init.hpp"

struct KeyNoteMapping {
    ImGuiKey key;
    uint8_t note_index;
};

static KeyNoteMapping key_note_mappings[] = {
    {ImGuiKey_Z, 0},  // C-0
    {ImGuiKey_S, 1},  // C#0
    {ImGuiKey_X, 2},  // D-0
    {ImGuiKey_D, 3},  // D#0
    {ImGuiKey_C, 4},  // E-0
    {ImGuiKey_V, 5},  // F-0
    {ImGuiKey_G, 6},  // F#0
    {ImGuiKey_B, 7},  // G-0
    {ImGuiKey_H, 8},  // G#0
    {ImGuiKey_N, 9},  // A-0
    {ImGuiKey_J, 10}, // A#0
    {ImGuiKey_M, 11}, // B-0

    {ImGuiKey_Comma, 12},     // C-1
    {ImGuiKey_L, 13},         // C#1
    {ImGuiKey_Period, 14},    // D-1
    {ImGuiKey_Semicolon, 15}, // D#1
    {ImGuiKey_Slash, 16},     // E-1

    {ImGuiKey_Q, 12}, // C-1
    {ImGuiKey_2, 13}, // C#1
    {ImGuiKey_W, 14}, // D-1
    {ImGuiKey_3, 15}, // D#1
    {ImGuiKey_E, 16}, // E-1
    {ImGuiKey_R, 17}, // F-1
    {ImGuiKey_5, 18}, // F#1
    {ImGuiKey_T, 19}, // G-1
    {ImGuiKey_6, 20}, // G#1
    {ImGuiKey_Y, 21}, // A-1
    {ImGuiKey_7, 22}, // A#1
    {ImGuiKey_U, 23}, // B-1

    {ImGuiKey_I, 24}, // C-2
    {ImGuiKey_9, 25}, // C#2
    {ImGuiKey_O, 26}, // D-2
    {ImGuiKey_0, 27}, // D#2
    {ImGuiKey_P, 28}  // E-2
};

#endif // GUI_MAPPING_HPP
