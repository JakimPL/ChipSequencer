#pragma once

#include <functional>
#include <map>

#include "../changes/selection.hpp"
#include "action.hpp"

class ChangePatternSelectionAction : public Action {
  private:
    PatternSelectionChange<uint8_t> selection_change;
    std::function<void(std::map<PatternRow, uint8_t>)> set_items;

  public:
    ChangePatternSelectionAction(
        const std::string &nm,
        GUIPanel *own,
        const LinkKey k,
        const PatternSelectionChange<uint8_t> &selection_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
    PatternRows get_pattern_rows() const;
};
