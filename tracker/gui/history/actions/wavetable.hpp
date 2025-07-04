#pragma once

#include "../changes/wavetable.hpp"
#include "action.hpp"

class ChangeWavetableAction : public Action {
  private:
    WavetableChange wavetable_change;

  public:
    ChangeWavetableAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        const WavetableChange &wt_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
