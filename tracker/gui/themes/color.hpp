#pragma once

#include <cstdint>
#include <string>

#include "../init.hpp"

struct ThemeColor {
    double red = 0.0;
    double green = 0.0;
    double blue = 0.0;
    double alpha = 1.0;

    ThemeColor() = default;
    explicit ThemeColor(double r, double g, double b, double a = 1.0);
    explicit ThemeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    explicit ThemeColor(const ImVec4 &color);
    explicit ThemeColor(const ImU32 &color);

    ThemeColor with_alpha(double alpha) const;
    ThemeColor with_alpha(uint8_t alpha) const;

    std::string to_hex() const;
    ImU32 to_u32() const;
    ImVec4 to_vec4() const;
    bool operator==(const ThemeColor &other) const;
    bool operator!=(const ThemeColor &other) const;

    static ThemeColor interpolate(const ThemeColor &color1, const ThemeColor &color2, float t);
};
