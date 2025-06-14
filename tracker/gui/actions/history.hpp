#pragma once

#include <deque>
#include <memory>

#include "../constants.hpp"
#include "action.hpp"

class HistoryManager {
  private:
    std::deque<std::unique_ptr<Action>> actions;
    const size_t max_history_size = GUI_MAX_HISTORY_SIZE;

    size_t current_index = 0;

  public:
    void add_action(std::unique_ptr<Action> action);
    void undo();
    void redo();
    bool can_undo() const;
    bool can_redo() const;
};
