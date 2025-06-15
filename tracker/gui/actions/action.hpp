#pragma once

#include <string>

#include "change.hpp"

class GUIPanel;

struct Action {
    Action(const std::string &nm, GUIPanel *own)
        : name(nm), owner(own) {}

    virtual ~Action() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo();
    virtual void notify_panel(const bool undo);

    const std::string name;
    GUIPanel *owner;
};

template <typename T>
class ChangeValueAction : public Action {
  private:
    ValueChange<T> value_change;

  public:
    ChangeValueAction(const std::string &nm, GUIPanel *own, const ValueChange<T> &val_ch)
        : Action(nm, own), value_change(val_ch) {}

    void execute() override {
        value_change.execute();
    }

    void undo() override {
        value_change.undo();
    }
};
