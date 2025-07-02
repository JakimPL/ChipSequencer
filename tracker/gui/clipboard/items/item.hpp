#pragma once

#include <string>

#include "../category.hpp"

struct ClipboardItem {
    ClipboardItem(ClipboardCategory cat, const std::string &nm, const std::string &h);
    const ClipboardCategory category;
    const std::string name;

    virtual std::string generate_hash() const = 0;
    std::string get_hash() const;

  private:
    const std::string hash;
};
