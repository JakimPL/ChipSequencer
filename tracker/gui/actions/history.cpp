#include "history.hpp"

void HistoryManager::add_action(std::unique_ptr<Action> action) {
    if (current_index < actions.size()) {
        actions.erase(actions.begin() + current_index, actions.end());
    }

    actions.push_back(std::move(action));
    current_index++;

    if (actions.size() > max_history_size) {
        actions.pop_front();
        current_index--;
    }
}

void HistoryManager::undo() {
    if (!can_undo()) {
        return;
    }

    current_index--;
    actions[current_index]->notify_panel(true);
}

void HistoryManager::redo() {
    if (!can_redo()) {
        return;
    }

    actions[current_index]->notify_panel(false);
    current_index++;
}

bool HistoryManager::can_undo() const {
    return current_index > 0;
}

bool HistoryManager::can_redo() const {
    return current_index < actions.size();
}
