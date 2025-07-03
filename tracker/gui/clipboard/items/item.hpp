#pragma once

#include <string>

#include "../category.hpp"

constexpr int MAX_CLIPBOARD_ENUMERATION = 10;

struct ClipboardItem {
    ClipboardItem(ClipboardCategory cat);
    virtual ~ClipboardItem() = default;

    const ClipboardCategory category;

    virtual void generate_hash() = 0;
    virtual void generate_name() = 0;

    const std::string &get_hash() const;
    const std::string &get_name() const;

  protected:
    std::string name;
    std::string hash;
};
