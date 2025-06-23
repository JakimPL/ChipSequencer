#include <sstream>

#include "../../names.hpp"
#include "remove.hpp"

template <typename T>
RemoveItemAction<T>::RemoveItemAction(
    const std::string &nm,
    GUIPanel *own,
    const LinkKey k,
    T *original_item,
    RemoveFunction remove,
    RestoreFunction<T> restore
)
    : Action(nm, own, k),
      remove_function(remove),
      restore_function(restore) {
    item_copy = resource_manager.allocate<T>();
    *item_copy = *original_item;
}

template <typename T>
RemoveItemAction<T>::~RemoveItemAction() {
    if (item_copy) {
        resource_manager.deallocate(item_copy);
        item_copy = nullptr;
    }
}

template <typename T>
void RemoveItemAction<T>::redo() {
    if (remove_function) {
        remove_function(item_index);
    }
}

template <typename T>
void RemoveItemAction<T>::undo() {
    if (restore_function && item_copy) {
        T *new_item = resource_manager.allocate<T>();
        *new_item = *item_copy;
        restore_function(new_item, item_index);
    }
}

template <typename T>
std::string RemoveItemAction<T>::get_name() const {
    std::ostringstream stream;
    stream << "Remove " << name;
    return stream.str();
}

template class RemoveItemAction<Envelope>;
template class RemoveItemAction<Sequence>;
template class RemoveItemAction<CommandsSequence>;
template class RemoveItemAction<Order>;
template class RemoveItemAction<Oscillator>;
template class RemoveItemAction<Wavetable>;
template class RemoveItemAction<DSP>;
template class RemoveItemAction<Channel>;
template class RemoveItemAction<CommandsChannel>;
