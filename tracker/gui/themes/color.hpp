#pragma once

#include "../init.hpp"

struct ThemeColor {
    double r = 0.0;
    double g = 0.0;
    double b = 0.0;
    double a = 1.0;

    explicit ThemeColor(double r, double g, double b, double a = 1.0);
    explicit ThemeColor(const ImVec4 &color);
    explicit ThemeColor(const ImU32 &color);

    ImU32 to_im_u32() const;
    ImVec4 to_im_vec4() const;
    bool operator==(const ThemeColor &other) const;
    bool operator!=(const ThemeColor &other) const;
};
