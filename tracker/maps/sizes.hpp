#pragma once

#include <string>
#include <unordered_map>

std::unordered_map<const char *, size_t> module_sizes = {
    {"Core", 2194},
    {"DSPs", 360},
    {"Commands", 492},
};

std::unordered_map<std::string, std::unordered_map<std::string, size_t>> component_sizes = {
    {
        "DSPs",
        {
            {"Gainer", 16},
            {"Distortion", 56},
            {"Filter", 140},
            {"Delay", 80},
        },
    },
    {
        "Oscillators",
        {
            {"Square", 48},
            {"Sine", 160},
            {"Saw", 68},
            {"Wavetable", 112},
            {"Noise", 80},
        },
    },
    {
        "Commands",
        {
            {"Portamento", 48},
            {"Portamento up", 16},
            {"Portamento down", 20},
            {"Set master gainer", 24},
            {"Set BPM", 20},
            {"Set division", 28},
            {"Load target", 12},
            {"Change 32bit value", 24},
            {"Change byte value", 12},
            {"Change word value", 16},
            {"Change dword value", 8},
            {"Change float value", 8},
            {"Add 32bit value", 24},
            {"Add byte value", 12},
            {"Add word value", 16},
            {"Add dword value", 8},
            {"Add float value", 8},
        },
    },
};
