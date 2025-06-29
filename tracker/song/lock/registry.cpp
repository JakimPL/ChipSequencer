#include "registry.hpp"

void LockRegistry::lock_item(const Target target, const size_t index) {
    locks[target].insert(index);
}

void LockRegistry::unlock_item(const Target target, const size_t index) {
    auto it = locks.find(target);
    if (it != locks.end()) {
        it->second.erase(index);
        if (it->second.empty()) {
            locks.erase(it);
        }
    }
}

bool LockRegistry::is_locked(const Target target, const size_t index) const {
    auto it = locks.find(target);
    if (it != locks.end()) {
        return it->second.find(index) != it->second.end();
    }
    return false;
}

bool LockRegistry::toggle_lock(const Target target, const size_t index) {
    if (is_locked(target, index)) {
        unlock_item(target, index);
        return false;
    } else {
        lock_item(target, index);
        return true;
    }
}

void LockRegistry::clear() {
    locks.clear();
}

void LockRegistry::realign_locks(const Target target, const size_t index, const bool removal) {
    auto it = locks.find(target);
    if (it == locks.end()) {
        return;
    }

    std::set<size_t> new_indices;
    for (size_t locked_index : it->second) {
        if (removal) {
            if (locked_index > index) {
                new_indices.insert(locked_index - 1);
            } else if (locked_index != index) {
                new_indices.insert(locked_index);
            }
        } else {
            if (locked_index >= index) {
                new_indices.insert(locked_index + 1);
            } else {
                new_indices.insert(locked_index);
            }
        }
    }

    if (new_indices.empty()) {
        locks.erase(it);
    } else {
        it->second = new_indices;
    }
}

std::set<size_t> LockRegistry::get_locked_items(const Target target) const {
    auto it = locks.find(target);
    if (it != locks.end()) {
        return it->second;
    }
    return {};
}

nlohmann::json LockRegistry::to_json() const {
    nlohmann::json json;
    for (const auto &[target, indices] : locks) {
        json[std::to_string(static_cast<int>(target))] = indices;
    }
    return json;
}

void LockRegistry::from_json(const nlohmann::json &json) {
    locks.clear();
    for (const auto &[key, value] : json.items()) {
        Target target = static_cast<Target>(std::stoi(key));
        std::set<size_t> indices = value.get<std::set<size_t>>();
        locks[target] = indices;
    }
}
