#ifndef STRUCTURES_RESOURCES_MANAGER_HPP
#define STRUCTURES_RESOURCES_MANAGER_HPP

#include <functional>
#include <unordered_map>

#include "../commands/command.hpp"
#include "../dsp.hpp"
#include "../oscillator.hpp"

class ResourceManager {
    std::unordered_map<void *, std::function<void(void *)>> deleters;

  public:
    ~ResourceManager() {
        clear();
    }

    template <typename T>
    T *allocate() {
        T *resource = new T();
        deleters[resource] = [](void *ptr) { delete static_cast<T *>(ptr); };
        return resource;
    }

    void deallocate(void *ptr) {
        if (deleters.count(ptr)) {
            deleters[ptr](ptr);
            deleters.erase(ptr);
        }
    }

    void clear() {
        for (auto &[ptr, deleter] : deleters) {
            deleter(ptr);
        }
        deleters.clear();
    }
};

#endif // STRUCTURES_RESOURCES_MANAGER_HPP
