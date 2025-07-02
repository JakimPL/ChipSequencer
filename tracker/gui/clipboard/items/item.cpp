#include "item.hpp"

ClipboardItem::ClipboardItem(ClipboardCategory cat, const std::string &nm, const std::string &h)
    : category(cat), name(nm), hash(h) {
}

std::string ClipboardItem::get_hash() const {
    return hash;
}
