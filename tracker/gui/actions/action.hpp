#pragma once

#include <iomanip>
#include <string>
#include <sstream>

#include "../../song/links/key.hpp"
#include "change.hpp"

class GUIPanel;

struct Action {
    Action(const std::string &nm, GUIPanel *own, const LinkKey k)
        : name(nm), owner(own), key(k) {}

    virtual ~Action() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual void redo();
    virtual void notify_panel(const bool undo);
    virtual bool can_merge(const Action *other) const {
        return key.target != Target::COUNT && key == other->key;
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
    ChangeValueAction(const std::string &nm, GUIPanel *own, const LinkKey k, const ValueChange<T> &val_ch)
        : Action(nm, own, k), value_change(val_ch) {}

    void execute() override {
        value_change.execute();
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
