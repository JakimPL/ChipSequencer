#pragma once

#include <cstddef>

#define TRACKER

constexpr size_t SIZE_CHANNEL = 16;
constexpr size_t SIZE_ENVELOPE = 14;

constexpr size_t SIZE_OSCILLATOR_SINE = 1;
constexpr size_t SIZE_OSCILLATOR_SQUARE = 3;
constexpr size_t SIZE_OSCILLATOR_SAW = 2;
constexpr size_t SIZE_OSCILLATOR_WAVETABLE = 3;
constexpr size_t SIZE_OSCILLATOR_NOISE = 1;

constexpr size_t SIZE_DSP = 11;
constexpr size_t SIZE_DSP_GAINER = 11;
constexpr size_t SIZE_DSP_DISTORTION = 11;
constexpr size_t SIZE_DSP_FILTER = 11;
constexpr size_t SIZE_DSP_DELAY = 15;

constexpr size_t SIZE_COMMAND_EMPTY = 2;
constexpr size_t SIZE_COMMAND_PORTAMENTO_UP = 5;
constexpr size_t SIZE_COMMAND_PORTAMENTO_DOWN = 5;
constexpr size_t SIZE_COMMAND_SET_MASTER_GAINER = 4;
constexpr size_t SIZE_COMMAND_SET_BPM = 4;
constexpr size_t SIZE_COMMAND_SET_DIVISION = 3;
constexpr size_t SIZE_COMMAND_CHANGE_FLOAT_VALUE = 8;
constexpr size_t SIZE_COMMAND_CHANGE_BYTE_VALUE = 5;
constexpr size_t SIZE_COMMAND_CHANGE_WORD_VALUE = 6;
constexpr size_t SIZE_COMMAND_CHANGE_DWORD_VALUE = 8;
constexpr size_t SIZE_COMMAND_ADD_FLOAT_VALUE = 8;
constexpr size_t SIZE_COMMAND_ADD_BYTE_VALUE = 5;
constexpr size_t SIZE_COMMAND_ADD_WORD_VALUE = 6;
constexpr size_t SIZE_COMMAND_ADD_DWORD_VALUE = 8;
constexpr size_t MAX_SIZE_COMMAND = 12;

constexpr size_t SIZE_COMMANDS_CHANNEL = 2;