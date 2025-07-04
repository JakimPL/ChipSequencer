#pragma once

#include "../changes/routing.hpp"
#include "action.hpp"

class ChangeRoutingAction : public Action {
  private:
    RoutingChange routing_change;

  public:
    ChangeRoutingAction(
        const std::string &nm,
        GUIPanel *own,
        LinkKey k,
        const RoutingChange &rout_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
