#pragma once

#include "../changes/order_item.hpp"
#include "action.hpp"

class ChangeOrderItemAction : public Action {
  private:
    OrderItemChange sequence_change;

  public:
    ChangeOrderItemAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        OrderItemChange seq_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
