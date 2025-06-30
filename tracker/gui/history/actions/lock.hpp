#pragma once

#include "action.hpp"

class ChangeLockAction : public Action {
  private:
    bool new_value;

  public:
    ChangeLockAction(
        const std::string &name,
        GUIPanel *owner,
        const LinkKey key,
        const bool value
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};