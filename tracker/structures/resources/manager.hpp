#pragma once

#include <functional>
#include <unordered_map>

class ResourceManager {
    std::unordered_map<void *, std::function<void(void *)>> deleters;

  public:
    ~ResourceManager() {
        clear();
    }

    template <typename T>
    T *allocate();

    template <typename T>
    T *allocateArray(size_t count);

    template <typename T, typename... Args>
    T *allocate(Args &&...args);

    void deallocate(void *ptr);
    void clear();
};
