#pragma once

#include "../changes/command.hpp"
#include "action.hpp"

class ChangeCommandAction : public Action {
  private:
    CommandChange command_change;

  public:
    ChangeCommandAction(
        const std::string &nm,
        GUIPanel *own,
        const LinkKey k,
        const CommandChange &cmd_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
    PatternRow get_pattern_row() const;
};
