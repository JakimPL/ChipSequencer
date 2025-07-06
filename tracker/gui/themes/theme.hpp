#pragma once

#include <nlohmann/json.hpp>

#include <map>
#include <string>

#include "color.hpp"
#include "item.hpp"

class Theme {
  private:
    std::map<ThemeItem, ThemeColor> colors;

  public:
    Theme();
    Theme(const Theme &other) = default;
    Theme &operator=(const Theme &other) = default;
    Theme(Theme &&other) = default;
    Theme &operator=(Theme &&other) = default;

    ThemeColor get_color(const ThemeItem item) const;
    ImU32 get_u32_color(const ThemeItem item) const;
    ImVec4 get_vec4_color(const ThemeItem item) const;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json &json);
};

std::string get_item_name(const ThemeItem item);
ThemeItem get_item_from_name(const std::string &str);
std::string color_to_hex(const ThemeColor &color);
ThemeColor hex_to_color(const std::string &hex);
