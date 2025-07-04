#include "item.hpp"

ClipboardItem::ClipboardItem(ClipboardCategory cat)
    : category(cat) {
}

const std::string &ClipboardItem::get_hash() const {
    return hash;
}

const std::string &ClipboardItem::get_name() const {
    return name;
}
