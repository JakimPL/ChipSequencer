#pragma once

#include <unordered_map>
#include <set>
#include <string>

#include "../../song/links/target.hpp"

class LockRegistry {
  private:
    std::unordered_map<Target, std::set<size_t>> locks;

  public:
    void lock_item(const Target target, const size_t index);
    void unlock_item(const Target target, const size_t index);
    void toggle_lock(const Target target, const size_t index);
    bool is_locked(const Target target, const size_t index) const;
    void clear();

    std::set<size_t> get_locked_items(const Target target) const;
};
