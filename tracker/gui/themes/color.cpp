#include <cmath>
#include <iomanip>
#include <sstream>

#include "color.hpp"

ThemeColor::ThemeColor(double r, double g, double b, double a)
    : r(r), g(g), b(b), a(a) {
}

ThemeColor::ThemeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : r(static_cast<double>(r) / 255.0),
      g(static_cast<double>(g) / 255.0),
      b(static_cast<double>(b) / 255.0),
      a(static_cast<double>(a) / 255.0) {
}

ThemeColor::ThemeColor(const ImVec4 &color)
    : r(color.x), g(color.y), b(color.z), a(color.w) {
}

ThemeColor::ThemeColor(const ImU32 &color)
    : r(((color >> IM_COL32_R_SHIFT) & 0xFF) / 255.0),
      g(((color >> IM_COL32_G_SHIFT) & 0xFF) / 255.0),
      b(((color >> IM_COL32_B_SHIFT) & 0xFF) / 255.0),
      a(((color >> IM_COL32_A_SHIFT) & 0xFF) / 255.0) {
}

ThemeColor ThemeColor::with_alpha(const double alpha) const {
    return ThemeColor(r, g, b, alpha);
}

ThemeColor ThemeColor::with_alpha(const uint8_t alpha) const {
    return with_alpha(static_cast<double>(alpha) / 255.0);
}

std::string ThemeColor::to_hex() const {
    const uint8_t r = static_cast<uint8_t>(std::round(r * 255.0));
    const uint8_t g = static_cast<uint8_t>(std::round(g * 255.0));
    const uint8_t b = static_cast<uint8_t>(std::round(b * 255.0));
    const uint8_t a = static_cast<uint8_t>(std::round(a * 255.0));

    const ImU32 u32_color = (r << 24) | (g << 16) | (b << 8) | a;

    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(8) << std::uppercase << u32_color;
    return ss.str();
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

ThemeColor ThemeColor::interpolate(const ThemeColor &color1, const ThemeColor &color2, const float t) {
    const float inv_t = 1.0f - t;
    return ThemeColor(
        inv_t * color1.r + t * color2.r,
        inv_t * color1.g + t * color2.g,
        inv_t * color1.b + t * color2.b,
        inv_t * color1.a + t * color2.a
    );
}

bool ThemeColor::operator==(const ThemeColor &other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

bool ThemeColor::operator!=(const ThemeColor &other) const {
    return !(*this == other);
}
