#ifndef SONG_HEADERS_HPP
#define SONG_HEADERS_HPP

#include <array>
#include <string>

#include "../structures/dsp.hpp"
#include "../structures/oscillator.hpp"
#include "../structures/commands/command.hpp"

std::array<std::string, static_cast<size_t>(Effect::Count)> header_effect_names = {
    "DSP_GAINER",
    "DSP_DISTORTION",
    "DSP_FILTER",
    "DSP_DELAY",
};

std::array<std::string, static_cast<size_t>(Generator::Count)> header_generator_names = {
    "OSCILLATOR_SQUARE",
    "OSCILLATOR_SAW",
    "OSCILLATOR_SINE",
    "OSCILLATOR_WAVETABLE",
    "OSCILLATOR_NOISE",
};

std::array<std::string, static_cast<size_t>(Instruction::Count)> header_instruction_names = {
    "COMMAND_EMPTY",
    "COMMAND_PORTAMENTO_UP",
    "COMMAND_PORTAMENTO_DOWN",
    "COMMAND_SET_MASTER_GAINER",
    "COMMAND_SET_BPM",
    "COMMAND_SET_DIVISION",
    "COMMAND_CHANGE_BYTE_VALUE",
    "COMMAND_CHANGE_WORD_VALUE",
    "COMMAND_CHANGE_DWORD_VALUE",
    "COMMAND_CHANGE_FLOAT_VALUE",
    "COMMAND_ADD_BYTE_VALUE",
    "COMMAND_ADD_WORD_VALUE",
    "COMMAND_ADD_DWORD_VALUE",
    "COMMAND_ADD_FLOAT_VALUE",
};

#endif // SONG_HEADERS_HPP
