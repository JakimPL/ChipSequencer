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

std::set<size_t> LockRegistry::get_locked_items(const Target target) const {
    auto it = locks.find(target);
    if (it != locks.end()) {
        return it->second;
    }
    return {};
}
