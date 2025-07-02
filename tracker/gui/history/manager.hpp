#pragma once

#include <deque>
#include <memory>
#include <string>

#include "../constants.hpp"
#include "actions/action.hpp"

class HistoryManager {
  private:
    std::deque<std::unique_ptr<Action>> actions;
    const size_t max_history_size = GUI_MAX_HISTORY_SIZE;

    size_t current_index = 0;

  public:
    HistoryManager() = default;
    ~HistoryManager() = default;
    HistoryManager(const HistoryManager &) = delete;
    HistoryManager &operator=(const HistoryManager &) = delete;
    HistoryManager(HistoryManager &&) = delete;
    HistoryManager &operator=(HistoryManager &&) = delete;

    void add_action(std::unique_ptr<Action> action);
    void undo();
    void redo();
    bool can_undo() const;
    bool can_redo() const;
    void clear();
    bool empty() const;

    size_t get_current_index() const;
    size_t get_history_size() const;
    std::string get_action_name(const size_t index) const;
    void go_to_index(size_t index);
};
