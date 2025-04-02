#ifndef GUI_MAPPING_HPP
#define GUI_MAPPING_HPP

#include "init.hpp"

static const char *note_names[] = {
    "C-0",
    "C#0",
    "D-0",
    "D#0",
    "E-0",
    "F-0",
    "F#0",
    "G-0",
    "G#0",
    "A-0",
    "A#0",
    "B-0",
    "C-1",
    "C#1",
    "D-1",
    "D#1",
    "E-1",
    "F-1",
    "F#1",
    "G-1",
    "G#1",
    "A-1",
    "A#1",
    "B-1",
    "C-2",
    "C#2",
    "D-2",
    "D#2",
    "E-2",
    "F-2",
    "F#2",
    "G-2",
    "G#2",
    "A-2",
    "A#2",
    "B-2",
    "C-3",
    "C#3",
    "D-3",
    "D#3",
    "E-3",
    "F-3",
    "F#3",
    "G-3",
    "G#3",
    "A-3",
    "A#3",
    "B-3",
    "C-4",
    "C#4",
    "D-4",
    "D#4",
    "E-4",
    "F-4",
    "F#4",
    "G-4",
    "G#4",
    "A-4",
    "A#4",
    "B-4",
    "C-5",
    "C#5",
    "D-5",
    "D#5",
    "E-5",
    "F-5",
    "F#5",
    "G-5",
    "G#5",
    "A-5",
    "A#5",
    "B-5",
    "C-6",
    "C#6",
    "D-6",
    "D#6",
    "E-6",
    "F-6",
    "F#6",
    "G-6",
    "G#6",
    "A-6",
    "A#6",
    "B-6",
    "C-7",
    "C#7",
    "D-7",
    "D#7",
    "E-7",
    "F-7",
    "F#7",
    "G-7",
    "G#7",
    "A-7",
    "A#7",
    "B-7",
    "C-8",
    "C#8",
    "D-8",
    "D#8",
    "E-8",
    "F-8",
    "F#8",
    "G-8",
    "G#8",
    "A-8",
    "A#8",
    "B-8",
    "C-9",
    "C#9",
    "D-9",
    "D#9",
    "E-9",
    "F-9",
    "F#9",
    "G-9",
    "G#9",
    "A-9",
    "A#9",
    "B-9",
    "C-10"
};

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
