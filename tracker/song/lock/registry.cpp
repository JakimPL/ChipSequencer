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

void LockRegistry::toggle_lock(const Target target, const size_t index) {
    if (is_locked(target, index)) {
        unlock_item(target, index);
    } else {
        lock_item(target, index);
    }
}

void LockRegistry::clear() {
    locks.clear();
}

void LockRegistry::realign_locks(const Target target, const size_t deleted_index) {
    auto it = locks.find(target);
    if (it == locks.end()) return;

    std::set<size_t> new_indices;
    for (size_t index : it->second) {
        if (index > deleted_index) {
            new_indices.insert(index - 1);
        } else if (index != deleted_index) {
            new_indices.insert(index);
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
