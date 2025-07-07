#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <map>
#include <string>

#include "color.hpp"
#include "item.hpp"

class Theme {
  private:
    std::map<ThemeItem, ThemeColor> colors;
    ThemeColor default_color = ThemeColor(0.0, 0.0, 0.0, 1.0);

  public:
    const std::filesystem::path themes_path;
    const std::filesystem::path current_theme_path;
    const std::filesystem::path default_theme_path;

    Theme();
    ~Theme();
    Theme(const Theme &other) = default;
    Theme &operator=(const Theme &other) = default;
    Theme(Theme &&other) = default;
    Theme &operator=(Theme &&other) = default;

    ThemeColor get_color(ThemeItem item) const;
    ImU32 get_u32_color(ThemeItem item) const;
    ImVec4 get_vec4_color(ThemeItem item) const;
    void set_color(ThemeItem item, const ThemeColor &color);

    void from_imgui();
    void to_imgui() const;

    nlohmann::json to_json() const;
    void from_json(const nlohmann::json &json);

    static std::string get_item_name(ThemeItem item);
    static ThemeItem get_item_from_name(const std::string &str);
    static std::string color_to_hex(const ThemeColor &color);
    ThemeColor hex_to_color(const std::string &hex);

    void load_current_theme();
    void load_theme(const std::filesystem::path &filepath);
    void save_current_theme() const;
    void save_theme(const std::filesystem::path &filepath) const;
};
