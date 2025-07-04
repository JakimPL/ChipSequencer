#pragma once

#include <deque>
#include <memory>
#include <unordered_map>

#include "category.hpp"
#include "items/item.hpp"

constexpr size_t MAX_CLIPBOARD_ITEMS = 10;

class Clipboard {
  public:
    Clipboard() = default;
    ~Clipboard() = default;
    Clipboard(const Clipboard &) = delete;
    Clipboard &operator=(const Clipboard &) = delete;
    Clipboard(Clipboard &&) = delete;
    Clipboard &operator=(Clipboard &&) = delete;

    void add_item(std::unique_ptr<ClipboardItem> item);
    ClipboardItem *get_recent_item(ClipboardCategory category) const;

    const std::deque<std::unique_ptr<ClipboardItem>> *get_items(ClipboardCategory category) const;
    void move_item_to_front(ClipboardCategory category, size_t index);

  private:
    std::unordered_map<ClipboardCategory, std::deque<std::unique_ptr<ClipboardItem>>> items;
};
