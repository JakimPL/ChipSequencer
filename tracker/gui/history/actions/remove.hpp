#pragma once

#include <functional>
#include <memory>

#include "../../../general.hpp"
#include "action.hpp"

using RemoveFunction = std::function<void(size_t)>;

template <typename T>
using RestoreFunction = std::function<T *(T *, size_t)>;

template <typename T>
class RemoveItemAction : public Action {
  private:
    RemoveFunction remove_function;
    RestoreFunction<T> restore_function;
    T item_copy;

  public:
    RemoveItemAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        const T &original_item,
        RemoveFunction remove,
        RestoreFunction<T> restore
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override { return false; }
    void merge(const Action *other) override {}
    std::string get_name() const override;
};