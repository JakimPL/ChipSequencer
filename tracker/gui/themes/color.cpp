#include "color.hpp"

ThemeColor::ThemeColor(double r, double g, double b, double a)
    : r(r), g(g), b(b), a(a) {
}

ThemeColor::ThemeColor(const ImVec4 &color)
    : r(color.x), g(color.y), b(color.z), a(color.w) {
}

ThemeColor::ThemeColor(const ImU32 &color)
    : r((color >> IM_COL32_R_SHIFT) / 255.0),
      g((color >> IM_COL32_G_SHIFT) / 255.0),
      b((color >> IM_COL32_B_SHIFT) / 255.0),
      a((color >> IM_COL32_A_SHIFT) / 255.0) {
}

ImU32 ThemeColor::to_u32() const {
    return IM_COL32(
        static_cast<unsigned char>(r * 255),
        static_cast<unsigned char>(g * 255),
        static_cast<unsigned char>(b * 255),
        static_cast<unsigned char>(a * 255)
    );
}

ImVec4 ThemeColor::to_vec4() const {
    return ImVec4(static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), static_cast<float>(a));
}

bool ThemeColor::operator==(const ThemeColor &other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

bool ThemeColor::operator!=(const ThemeColor &other) const {
    return !(*this == other);
}
