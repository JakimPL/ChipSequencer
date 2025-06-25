#pragma once

#include <functional>
#include "action.hpp"

using AddFunction = std::function<void *()>;
using RemoveFunction = std::function<void(size_t)>;

class AddItemAction : public Action {
  private:
    AddFunction add_function;
    RemoveFunction remove_function;

  public:
    AddItemAction(
        const std::string &nm,
        GUIPanel *own,
        const LinkKey k,
        AddFunction add,
        RemoveFunction remove
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
