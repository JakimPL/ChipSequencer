#pragma once

#include <string>

#include "../category.hpp"

struct ClipboardItem {
    ClipboardItem(ClipboardCategory cat, const std::string &nm);
    virtual ~ClipboardItem() = default;

    const ClipboardCategory category;
    const std::string name;

    virtual void generate_hash() = 0;
    std::string get_hash() const;

  protected:
    std::string hash;
};
