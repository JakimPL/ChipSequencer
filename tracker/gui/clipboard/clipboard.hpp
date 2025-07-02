#pragma once

#include <deque>
#include <memory>
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

    void add_item(std::unique_ptr<ClipboardItem> item);

  private:
    std::unordered_map<ClipboardCategory, std::deque<std::unique_ptr<ClipboardItem>>> items;
};
