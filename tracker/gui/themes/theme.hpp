#pragma once

#include <unordered_map>

#include "color.hpp"
#include "item.hpp"

class Theme {
  private:
    std::unordered_map<ThemeItem, ThemeColor> colors;

  public:
    Theme();
    Theme(const Theme &other) = default;
    Theme &operator=(const Theme &other) = default;
    Theme(Theme &&other) = default;
    Theme &operator=(Theme &&other) = default;

    ThemeColor get_color(const ThemeItem item) const;
    ImU32 get_u32_color(const ThemeItem item) const;
    ImVec4 get_vec4_color(const ThemeItem item) const;
};
