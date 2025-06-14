#include "../init.hpp"
#include "selection.hpp"

void PatternSelection::select(
    const int start,
    const int end,
    const bool command,
    const size_t channel_start,
    const size_t channel_end
) {
    this->command = command;
    this->channel_index = channel_index;
    this->start = start;
    this->end = end;
    this->channel_start = channel_start;
    this->channel_end = channel_end;
    selecting = false;
    adjust_selection();
}

bool PatternSelection::is_row_selected(const size_t channel_index, const int row) const {
    const int min = std::min(start, end);
    const int max = std::max(start, end);
    const int channel_min = std::min(channel_start, channel_end);
    const int channel_max = std::max(channel_start, channel_end);
    return (
        row >= min && row <= max &&
        channel_index >= channel_min && channel_index <= channel_max
    );
};

void PatternSelection::form(const bool commands, const size_t channel_index, const int row) {
    if (
        ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId) ||
        !ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)
    ) {
        return;
    }

    if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        if (!selecting) {
            command = commands;
            selecting = true;
            start = row;
            end = row;
            channel_start = channel_index;
            channel_end = channel_index;
        } else if (commands == command) {
            end = row;
            channel_end = channel_index;
        }
    } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        selecting = false;
        if (channel_start == channel_end && start == end) {
            clear();
            return;
        }

        adjust_selection();
    }
}

void PatternSelection::clear() {
    selecting = false;
    command = false;
    start = -1;
    end = -1;
    channel_start = -1;
    channel_end = -1;
}

bool PatternSelection::is_active() const {
    return start != -1 && end != -1 && channel_start != -1 && channel_end != -1;
}

void PatternSelection::adjust_selection() {
    if (start > end) {
        std::swap(start, end);
    }

    if (channel_start > channel_end) {
        std::swap(channel_start, channel_end);
    }
}
