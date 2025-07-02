#pragma once

#include <string>

#include "category.hpp"

struct ClipboardItem {
    ClipboardCategory category;
    std::string name;
    std::string hash;
};
