#include "../init.hpp"
#include "selection.hpp"

bool PatternSelection::is_row_selected(const size_t channel_index, const int row) const {
    return (
        row >= start && row <= end &&
        channel_index >= channel_start && channel_index <= channel_end
    );
};

void PatternSelection::form(const size_t channel_index, const int row) {
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly)) {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            if (!selecting) {
                selecting = true;
                start = row;
                end = row;
                channel_start = channel_index;
                channel_end = channel_index;
            } else {
                end = row;
                channel_end = channel_index;
            }
        } else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            selecting = false;
        }
    }

    if (!selecting && channel_start == channel_end && start == end) {
        reset();
        return;
    }

    if (start > end) {
        std::swap(start, end);
    }

    if (channel_start > channel_end) {
        std::swap(channel_start, channel_end);
    }
}

void PatternSelection::reset() {
    selecting = false;
    start = -1;
    end = -1;
    channel_start = -1;
    channel_end = -1;
}
