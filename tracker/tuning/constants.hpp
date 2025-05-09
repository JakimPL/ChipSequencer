#ifndef TUNING_CONSTANTS_HPP
#define TUNING_CONSTANTS_HPP

#define MIN_EDO 7
#define MAX_EDO 32
#define MIN_A4_FREQUENCY 400.0
#define MAX_A4_FREQUENCY 600.0

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

inline static const char *scale_symbols[] = {
    "b",
    "",
    "#",
};

#endif // TUNING_CONSTANTS_HPP
