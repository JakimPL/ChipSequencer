#pragma once

#include <string>

class Action {
  public:
    virtual ~Action() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo() {
        execute();
    };

    virtual std::string get_name() const = 0;
};

template <typename T>
class ChangeValueAction : public Action {
  private:
    T &parameter;
    const T old_value;
    const T new_value;
    const std::string action_name;

  public:
    ChangeValueAction(T &param, T old_val, T new_val, const std::string &name)
        : parameter(param), old_value(old_val), new_value(new_val), action_name(name) {}

    void execute() override {
        parameter = new_value;
    }

    void undo() override {
        parameter = old_value;
    }

    std::string get_name() const override {
        return action_name;
    }
};
