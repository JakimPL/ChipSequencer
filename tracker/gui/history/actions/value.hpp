#pragma once

#include "../changes/value.hpp"
#include "action.hpp"

template <typename T>
class ChangeValueAction : public Action {
  private:
    ValueChange<T> value_change;

  public:
    ChangeValueAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        const ValueChange<T> &val_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
