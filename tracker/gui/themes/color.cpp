#include <cmath>
#include <iomanip>
#include <sstream>

#include "color.hpp"

ThemeColor::ThemeColor(double r, double g, double b, double a)
    : red(r), green(g), blue(b), alpha(a) {
}

ThemeColor::ThemeColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    : red(static_cast<double>(r) / 255.0),
      green(static_cast<double>(g) / 255.0),
      blue(static_cast<double>(b) / 255.0),
      alpha(static_cast<double>(a) / 255.0) {
}

ThemeColor::ThemeColor(const ImVec4 &color)
    : red(color.x), green(color.y), blue(color.z), alpha(color.w) {
}

ThemeColor::ThemeColor(const ImU32 &color)
    : red(((color >> IM_COL32_R_SHIFT) & 0xFF) / 255.0),
      green(((color >> IM_COL32_G_SHIFT) & 0xFF) / 255.0),
      blue(((color >> IM_COL32_B_SHIFT) & 0xFF) / 255.0),
      alpha(((color >> IM_COL32_A_SHIFT) & 0xFF) / 255.0) {
}

ThemeColor ThemeColor::with_alpha(const double alpha) const {
    return ThemeColor(red, green, blue, alpha);
}

ThemeColor ThemeColor::with_alpha(const uint8_t alpha) const {
    return with_alpha(static_cast<double>(alpha) / 255.0);
}

std::string ThemeColor::to_hex() const {
    const uint8_t r = static_cast<uint8_t>(std::round(red * 255.0));
    const uint8_t g = static_cast<uint8_t>(std::round(green * 255.0));
    const uint8_t b = static_cast<uint8_t>(std::round(blue * 255.0));
    const uint8_t a = static_cast<uint8_t>(std::round(alpha * 255.0));

    const ImU32 u32_color = (r << 24) | (g << 16) | (b << 8) | a;

    std::stringstream ss;
    ss << "#" << std::hex << std::setfill('0') << std::setw(8) << std::uppercase << u32_color;
    return ss.str();
}

ImU32 ThemeColor::to_u32() const {
    return IM_COL32(
        static_cast<unsigned char>(red * 255),
        static_cast<unsigned char>(green * 255),
        static_cast<unsigned char>(blue * 255),
        static_cast<unsigned char>(alpha * 255)
    );
}

ImVec4 ThemeColor::to_vec4() const {
    return ImVec4(
        static_cast<float>(red),
        static_cast<float>(green),
        static_cast<float>(blue),
        static_cast<float>(alpha)
    );
}

ThemeColor ThemeColor::interpolate(const ThemeColor &color1, const ThemeColor &color2, const float t) {
    const float inv_t = 1.0f - t;
    return ThemeColor(
        inv_t * color1.red + t * color2.red,
        inv_t * color1.green + t * color2.green,
        inv_t * color1.blue + t * color2.blue,
        inv_t * color1.alpha + t * color2.alpha
    );
}

bool ThemeColor::operator==(const ThemeColor &other) const {
    return red == other.red && green == other.green && blue == other.blue && alpha == other.alpha;
}

bool ThemeColor::operator!=(const ThemeColor &other) const {
    return !(*this == other);
}
