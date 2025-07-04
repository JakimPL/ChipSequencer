#pragma once

#include <functional>
#include <map>

#include "../changes/selection.hpp"
#include "action.hpp"

template <typename T>
using SetItemsFunction = std::function<void(std::map<PatternRow, T>)>;

template <typename T>
class ChangePatternSelectionAction : public Action {
  private:
    PatternSelectionChange<T> selection_changes;
    SetItemsFunction<T> set_items;

  public:
    ChangePatternSelectionAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        const PatternSelectionChange<T> &s_ch,
        SetItemsFunction<T> set_items_function
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
