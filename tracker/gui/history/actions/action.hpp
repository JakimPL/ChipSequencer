#pragma once

#include <string>

#include "../../../song/links/key.hpp"

class GUIPanel;

struct Action {
    Action(std::string nm, GUIPanel *own, LinkKey k);
    virtual ~Action() = default;

    virtual void undo() = 0;
    virtual void redo();
    virtual void execute();
    virtual void notify_panel(bool undo);
    virtual bool can_merge(const Action *other) const;
    virtual void merge(const Action *other);
    virtual std::string get_name() const;

    const LinkKey key;
    const std::string name;
    GUIPanel *owner;
};
