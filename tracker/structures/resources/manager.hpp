#pragma once

#include <memory>
#include <unordered_map>

class ResourceManager {
    // Base class for type erasure
    struct ResourceBase {
        virtual ~ResourceBase() = default;
    };

    // Templated derived class that holds the actual unique_ptr
    template <typename T>
    struct Resource : ResourceBase {
        std::unique_ptr<T> ptr;
        explicit Resource(std::unique_ptr<T> p)
            : ptr(std::move(p)) {}
    };

    std::unordered_map<void *, std::unique_ptr<ResourceBase>> resources;

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
