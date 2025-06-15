#include "history.hpp"

void HistoryManager::add_action(std::unique_ptr<Action> action) {
    if (current_index > 0) {
        Action *last_action = actions[current_index - 1].get();

        if (last_action->can_merge(action.get())) {
            last_action->merge(action.get());
            return;
        }
    }

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

void HistoryManager::clear() {
    actions.clear();
    current_index = 0;
}

size_t HistoryManager::get_current_index() const {
    return current_index;
}
size_t HistoryManager::get_history_size() const {
    return actions.size();
}

std::string HistoryManager::get_action_name(const size_t index) const {
    if (index < actions.size()) {
        return actions[index]->get_name();
    }
    return "Unknown";
}

void HistoryManager::go_to_index(size_t index) {
    if (index > actions.size()) {
        index = actions.size();
    }

    while (current_index < index) {
        redo();
    }

    while (current_index > index) {
        undo();
    }
}
