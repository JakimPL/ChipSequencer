#pragma once

#include "../changes/sequence.hpp"
#include "action.hpp"

class ChangeOrderSequenceAction : public Action {
  private:
    OrderSequenceChange sequence_change;

  public:
    ChangeOrderSequenceAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        OrderSequenceChange seq_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
