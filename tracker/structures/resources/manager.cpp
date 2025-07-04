#include "../../structures.hpp"
#include "manager.hpp"

ResourceManager::~ResourceManager() {
    clear();
}

template <typename T, typename... Args>
T *ResourceManager::allocate(Args &&...args) {
    auto unique_ptr = std::make_unique<T>(std::forward<Args>(args)...);
    T *raw_ptr = unique_ptr.get();

    auto resource = std::make_unique<Resource<T>>(std::move(unique_ptr));
    resources[raw_ptr] = std::move(resource);

    return raw_ptr;
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
