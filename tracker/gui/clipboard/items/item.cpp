#include "item.hpp"

ClipboardItem::ClipboardItem(ClipboardCategory cat, const std::string &nm)
    : category(cat), name(nm) {
}

std::string ClipboardItem::get_hash() const {
    return hash;
}
