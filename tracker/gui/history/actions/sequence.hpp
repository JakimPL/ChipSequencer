#pragma once

#include "../changes/sequence.hpp"
#include "action.hpp"

template <typename T, typename Panel>
class ChangeSequenceAction : public Action {
  private:
    SequenceChange<T> sequence_change;

  public:
    ChangeSequenceAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        SequenceChange<T> seq_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
