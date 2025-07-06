#pragma once

#include <unordered_map>

#include "color.hpp"
#include "item.hpp"

class Theme {
  private:
    std::unordered_map<ThemeColorItem, ThemeColor> colors;

  public:
    Theme() = default;
    Theme(const Theme &other) = default;
    Theme &operator=(const Theme &other) = default;
    Theme(Theme &&other) = default;
    Theme &operator=(Theme &&other) = default;
};
