#pragma once

#include <deque>
#include <unordered_map>

#include "category.hpp"
#include "items/item.hpp"

class Clipboard {
  public:
    Clipboard() = default;
    ~Clipboard() = default;
    Clipboard(const Clipboard &) = delete;
    Clipboard &operator=(const Clipboard &) = delete;
    Clipboard(Clipboard &&) = delete;
    Clipboard &operator=(Clipboard &&) = delete;

    std::unordered_map<ClipboardCategory, std::deque<ClipboardItem>> items;
};
