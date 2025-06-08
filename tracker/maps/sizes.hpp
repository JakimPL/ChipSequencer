#ifndef MAPS_SIZES_HPP
#define MAPS_SIZES_HPP

#include <string>
#include <unordered_map>

std::unordered_map<const char *, size_t> module_sizes = {
    {"Core", 2184},
    {"DSPs", 360},
    {"Commands", 488},
};

std::unordered_map<std::string, std::unordered_map<std::string, size_t>> component_sizes = {
    {
        "DSPs",
        {
            {"Gainer", 16},
            {"Distortion", 56},
            {"Filter", 156},
            {"Delay", 80},
        },
    },
    {
        "Oscillators",
        {
            {"Square", 48},
            {"Sine", 156},
            {"Saw", 68},
            {"Wavetable", 112},
            {"Noise", 80},
        },
    },
    {
        "Commands",
        {
            {"Portamento up", 68},
            {"Portamento down", 68},
            {"Set master gainer", 28},
            {"Set BPM", 20},
            {"Set division", 32},
            {"Change byte value", 28},
            {"Change word value", 28},
            {"Change dword value", 32},
            {"Change float value", 32},
            {"Add byte value", 28},
            {"Add word value", 28},
            {"Add dword value", 32},
            {"Add float value", 32},
        },
    },
};

#endif // MAPS_SIZES_HPP
