#pragma once

#include "../changes/text.hpp"
#include "action.hpp"

template <size_t n>
class ChangeTextAction : public Action {
  private:
    TextChange<n> text_change;

  public:
    ChangeTextAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        TextChange<n> txt_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
