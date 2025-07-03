#include <algorithm>

#include "clipboard.hpp"

void Clipboard::add_item(std::unique_ptr<ClipboardItem> item) {
    if (!item) {
        return;
    }

    auto &category_items = items[item->category];
    const std::string &hash = item->get_hash();

    auto it = std::find_if(category_items.begin(), category_items.end(), [&hash](const std::unique_ptr<ClipboardItem> &existing_item) {
        return existing_item->get_hash() == hash;
    });

    if (it != category_items.end()) {
        auto existing_item = std::move(*it);
        category_items.erase(it);
        category_items.push_front(std::move(existing_item));
    } else {
        category_items.push_front(std::move(item));
        if (category_items.size() > MAX_CLIPBOARD_ITEMS) {
            category_items.pop_back();
        }
    }
}

ClipboardItem *Clipboard::get_recent_item(const ClipboardCategory category) const {
    auto it = items.find(category);
    if (it != items.end() && !it->second.empty()) {
        return it->second.front().get();
    }

    return nullptr;
}

const std::deque<std::unique_ptr<ClipboardItem>> *Clipboard::get_items(const ClipboardCategory category) const {
    auto it = items.find(category);
    if (it != items.end()) {
        return &it->second;
    }

    return nullptr;
}

void Clipboard::move_item_to_front(const ClipboardCategory category, size_t index) {
    auto it = items.find(category);
    if (it == items.end() || index >= it->second.size()) {
        return;
    }

    auto &category_items = it->second;
    if (index == 0) {
        return;
    }

    auto item = std::move(category_items[index]);
    category_items.erase(category_items.begin() + index);
    category_items.push_front(std::move(item));
}
