#pragma once

#include <iomanip>
#include <string>
#include <sstream>

#include "../../maps/routing.hpp"
#include "../../song/links/key.hpp"
#include "change.hpp"

class GUIPanel;

struct Action {
    Action(const std::string &nm, GUIPanel *own, const LinkKey k)
        : name(nm), owner(own), key(k) {}

    virtual ~Action() = default;
    virtual void undo() = 0;
    virtual void redo() = 0;
    virtual void execute() { redo(); };
    virtual void notify_panel(const bool undo);
    virtual bool can_merge(const Action *other) const {
        return key == other->key && !key.is_null();
    }
    virtual void merge(const Action *other) {}
    virtual std::string get_name() const { return name; }

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
    )
        : Action(nm, own, k), value_change(val_ch) {}

    void redo() override {
        value_change.redo();
    }

    void undo() override {
        value_change.undo();
    }

    bool can_merge(const Action *other) const override {
        if (!Action::can_merge(other)) {
            return false;
        }

        return dynamic_cast<const ChangeValueAction<T> *>(other) != nullptr;
    }

    void merge(const Action *other) override {
        const auto *other_change = dynamic_cast<const ChangeValueAction<T> *>(other);
        if (other_change) {
            value_change.new_value = other_change->value_change.new_value;
        }
    }

    std::string get_name() const override {
        std::ostringstream stream;
        stream << "Change " << name;
        if constexpr (std::is_floating_point_v<T>) {
            stream << " from " << std::setprecision(4) << value_change.old_value
                   << " to " << std::setprecision(4) << value_change.new_value;
        } else {
            stream << " from " << value_change.old_value
                   << " to " << value_change.new_value;
        }
        return stream.str();
    }
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
    )
        : Action(nm, own, k), routing_change(rout_ch) {}

    void redo() override {
        routing_change.redo();
    }

    void undo() override {
        routing_change.undo();
    }

    bool can_merge(const Action *other) const override {
        if (!Action::can_merge(other)) {
            return false;
        }

        return dynamic_cast<const ChangeRoutingAction *>(other) != nullptr;
    }

    void merge(const Action *other) override {
        const auto *other_change = dynamic_cast<const ChangeRoutingAction *>(other);
        if (other_change) {
            routing_change.new_value = other_change->routing_change.new_value;
        }
    }

    std::string get_name() const override {
        std::ostringstream stream;
        stream << "Change routing for " << name
               << " from " << get_key_name(routing_change.old_value)
               << " to " << get_key_name(routing_change.new_value);
        return stream.str();
    }
};
