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
        deleters[resource] = [](void *ptr) { delete static_cast<T *>(ptr); };
        return resource;
    } catch (...) {
        delete resource;
        throw;
    }
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
