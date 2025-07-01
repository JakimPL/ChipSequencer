#include "../../structures.hpp"
#include "manager.hpp"

template <typename T>
T *ResourceManager::allocate() {
    T *resource = new T();
    deleters[resource] = [](void *ptr) { delete static_cast<T *>(ptr); };
    return resource;
}

template <typename T>
T *ResourceManager::allocateArray(size_t count) {
    T *resource = new T[count];
    deleters[resource] = [](void *ptr) { delete[] static_cast<T *>(ptr); };
    return resource;
}

template <typename T, typename... Args>
T *ResourceManager::allocate(Args &&...args) {
    T *resource = new T(std::forward<Args>(args)...);
    deleters[resource] = [](void *ptr) { delete static_cast<T *>(ptr); };
    return resource;
}

void ResourceManager::deallocate(void *ptr) {
    if (deleters.count(ptr)) {
        deleters[ptr](ptr);
        deleters.erase(ptr);
    }
}

void ResourceManager::clear() {
    for (auto &[ptr, deleter] : deleters) {
        deleter(ptr);
    }
    deleters.clear();
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
