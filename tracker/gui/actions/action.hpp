#pragma once

#include <string>

#include "../../maps/routing.hpp"
#include "../../song/links/key.hpp"
#include "change.hpp"

class GUIPanel;

struct Action {
    Action(const std::string &nm, GUIPanel *own, const LinkKey k);
    virtual ~Action() = default;

    virtual void undo() = 0;
    virtual void redo();
    virtual void execute();
    virtual void notify_panel(const bool undo);
    virtual bool can_merge(const Action *other) const;
    virtual void merge(const Action *other);
    virtual std::string get_name() const;

    const LinkKey key;
    const std::string name;
    GUIPanel *owner;
};

template <typename T>
class ChangeValueAction : public Action {
  private:
    ValueChange<T> value_change;

  public:
    ChangeValueAction(
        const std::string &nm,
        GUIPanel *own,
        const LinkKey k,
        const ValueChange<T> &val_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};

class ChangeRoutingAction : public Action {
  private:
    ValueChange<LinkKey> routing_change;

  public:
    ChangeRoutingAction(
        const std::string &nm,
        GUIPanel *own,
        const LinkKey k,
        const ValueChange<LinkKey> &rout_ch
    );

    void redo() override;
    void undo() override;
    bool can_merge(const Action *other) const override;
    void merge(const Action *other) override;
    std::string get_name() const override;
};
