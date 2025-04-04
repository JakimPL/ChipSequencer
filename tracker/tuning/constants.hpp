#ifndef TUNING_CONSTANTS_HPP
#define TUNING_CONSTANTS_HPP

#define MIN_EDO 7
#define MAX_EDO 32

constexpr double range_mean = 632.4555320336759; // sqrt(20 * 20_000)

inline static const char *scale_names[] = {
    "C",
    "D",
    "E",
    "F",
    "G",
    "A",
    "B",
};

static constexpr int scale_intervals_count = 8;
inline static const double scale_intervals[] = {
    1.0,
    9.0 / 8.0,
    5.0 / 4.0,
    4.0 / 3.0,
    3.0 / 2.0,
    5.0 / 3.0,
    15.0 / 8.0,
    2.0,
};

inline static const std::pair<int, const char *> scale_symbols[] = {
    {-1, "b"},
    {0, ""},
    {1, "#"},
};

#endif // TUNING_CONSTANTS_HPP
