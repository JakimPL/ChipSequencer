#pragma once

#include <functional>
#include <memory>
#include <unordered_map>

class ResourceManager {
    std::unordered_map<void *, std::unique_ptr<void, std::function<void(void *)>>> resources;

  public:
    ResourceManager() = default;
    ~ResourceManager();
    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;
    ResourceManager(ResourceManager &&) = delete;
    ResourceManager &operator=(ResourceManager &&) = delete;

    template <typename T, typename... Args>
    T *allocate(Args &&...args);

    void deallocate(void *ptr);
    void clear();
};
