#pragma once

#include "../changes/sequence.hpp"
#include "action.hpp"

class ChangeSequenceAction : public Action {
  private:
    SequenceChange sequence_change;

  public:
    ChangeSequenceAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        SequenceChange seq_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
