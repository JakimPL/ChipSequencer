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
