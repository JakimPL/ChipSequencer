#pragma once

#include <unordered_map>

class ClipboardManager {
  public:
    ClipboardManager() = default;
    ~ClipboardManager() = default;
    ClipboardManager(const ClipboardManager &) = delete;
    ClipboardManager &operator=(const ClipboardManager &) = delete;
    ClipboardManager(ClipboardManager &&) = delete;
    ClipboardManager &operator=(ClipboardManager &&) = delete;
};
