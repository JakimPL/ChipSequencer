#include "../../structures.hpp"
#include "manager.hpp"

ResourceManager::~ResourceManager() {
    clear();
}

template <typename T, typename... Args>
T *ResourceManager::allocate(Args &&...args) {
    T *resource = nullptr;
    try {
        resource = new T(std::forward<Args>(args)...);
        auto deleter = [](void *ptr) { delete static_cast<T *>(ptr); };
        auto unique_ptr = std::unique_ptr<void, std::function<void(void *)>>(resource, deleter);

        void *raw_ptr = resource;
        resources[raw_ptr] = std::move(unique_ptr);

        return resource;
    } catch (...) {
        delete resource;
        throw;
    }
}

void ResourceManager::deallocate(void *ptr) {
    if (resources.count(ptr) != 0u) {
        resources.erase(ptr);
    }
}

void ResourceManager::clear() {
    resources.clear();
}

template Envelope *ResourceManager::allocate<Envelope>();
template Sequence *ResourceManager::allocate<Sequence>();
template CommandsSequence *ResourceManager::allocate<CommandsSequence>();
template Order *ResourceManager::allocate<Order>();
template Oscillator *ResourceManager::allocate<Oscillator>();
template Wavetable *ResourceManager::allocate<Wavetable>();
template DSP *ResourceManager::allocate<DSP>();
template Channel *ResourceManager::allocate<Channel>();
template CommandsChannel *ResourceManager::allocate<CommandsChannel>();

template OscillatorSquare *ResourceManager::allocate<OscillatorSquare>();
template OscillatorSine *ResourceManager::allocate<OscillatorSine>();
template OscillatorSaw *ResourceManager::allocate<OscillatorSaw>();
template OscillatorWavetable *ResourceManager::allocate<OscillatorWavetable>();
template OscillatorNoise *ResourceManager::allocate<OscillatorNoise>();

template DSPGainer *ResourceManager::allocate<DSPGainer>();
template DSPDistortion *ResourceManager::allocate<DSPDistortion>();
template DSPFilter *ResourceManager::allocate<DSPFilter>();
template DSPDelay *ResourceManager::allocate<DSPDelay>();
