#include "clipboard.hpp"

void Clipboard::add_item(std::unique_ptr<ClipboardItem> item) {
    if (!item) {
        return;
    }

    auto &category_items = items[item->category];
    category_items.push_back(std::move(item));
}
