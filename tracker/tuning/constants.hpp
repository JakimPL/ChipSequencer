#pragma once

#define MIN_EDO 7
#define MAX_EDO 32
#define MIN_A4_FREQUENCY 400.0
#define MAX_A4_FREQUENCY 600.0

#include <array>
#include <utility>

constexpr double range_mean = 632.4555320336759; // sqrt(20 * 20_000)

inline static const char *scale_names[] = {
    "F",
    "C",
    "G",
    "D",
    "A",
    "E",
    "B",
};

inline static const char *scale_names_ordered[] = {
    "C",
    "D",
    "E",
    "F",
    "G",
    "A",
    "B",
};

constexpr std::array<const char *, 3> scale_symbols = {
    "b",
    "",
    "#",
};
