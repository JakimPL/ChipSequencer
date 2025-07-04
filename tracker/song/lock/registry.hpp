#pragma once

#include <unordered_map>
#include <set>
#include <string>

#include <nlohmann/json.hpp>

#include "../../song/links/target.hpp"

class LockRegistry {
  private:
    std::unordered_map<Target, std::set<size_t>> locks;

  public:
    void lock_item(Target target, size_t index);
    void unlock_item(Target target, size_t index);
    bool toggle_lock(Target target, size_t index);
    bool is_locked(Target target, size_t index) const;
    void clear();

    void realign_locks(Target target, size_t index, bool removal);
    std::set<size_t> get_locked_items(Target target) const;

    void from_json(const nlohmann::json &json);
    nlohmann::json to_json() const;
};
