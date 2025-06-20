#pragma once

#include <functional>
#include "action.hpp"

typedef std::function<void *()> AddFunction;
typedef std::function<void(size_t)> RemoveFunction;

class AddItemAction : public Action {
  private:
    Target type;
    size_t index;
    bool first_redo;

    AddFunction add_function;
    RemoveFunction remove_function;

  public:
    AddItemAction(
        const std::string &nm,
        GUIPanel *own,
        const LinkKey k,
        Target t,
        AddFunction add,
        RemoveFunction remove
    );

    void redo() override;
    void undo() override;
    std::string get_name() const override;
};
