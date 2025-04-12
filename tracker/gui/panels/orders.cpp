#include "../../general.hpp"
#include "../enums.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "orders.hpp"

GUIOrdersPanel::GUIOrdersPanel() {
    from();
    update();
}

void GUIOrdersPanel::draw() {
    ImGui::Begin("Order Editor");
    ImGui::Columns(1, "order_columns");

    push_tertiary_style();
    draw_add_or_remove();
    prepare_combo(order_names, "##OrderCombo", order_index);
    pop_tertiary_style();

    ImGui::Separator();

    from();
    draw_order();
    check_keyboard_input();
    to();

    ImGui::Columns(1);
    ImGui::End();
}

bool GUIOrdersPanel::is_index_valid() const {
    return order_index >= 0 && order_index < orders.size();
}

void GUIOrdersPanel::from() {
    if (!is_index_valid()) {
        return;
    }

    const Order *order = orders[order_index];
    uint16_t total_length = order->order_length;
    current_order.sequences.resize(total_length);
    current_order.length = total_length;
    std::copy(order->sequences.begin(), order->sequences.begin() + total_length, current_order.sequences.begin());
}

void GUIOrdersPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    Order *order = orders[order_index];
    const size_t length = current_order.length;
    order->order_length = length;
    for (size_t i = 0; i < length; ++i) {
        if (i < current_order.sequences.size()) {
            order->sequences[i] = current_order.sequences[i];
        } else {
            order->sequences[i] = 0;
        }
    }
}

void GUIOrdersPanel::add() {
    Order *new_order = song.add_order();
    if (new_order == nullptr) {
        return;
    }

    order_index = orders.size() - 1;
    update();
}

void GUIOrdersPanel::remove() {
    if (is_index_valid()) {
        song.remove_order(order_index);
        order_index = std::max(0, order_index - 1);
        update();
    }
}

void GUIOrdersPanel::update() {
    update_items(order_names, orders.size(), "Order ", order_index);
    gui.update(GUIElement::Channels);
}

void GUIOrdersPanel::draw_order_length() {
    draw_number_of_items("Sequences", "##SequenceLength", current_order.length, 1, max_items);
}

void GUIOrdersPanel::draw_order() {
    if (!is_index_valid()) {
        ImGui::Text("No order available.");
        return;
    }

    ImGui::Text("Order:");

    draw_order_length();

    const float height = std::max(5.0f, ImGui::GetContentRegionAvail().y - 5.0f);
    ImGui::BeginChild("OrderScroll", ImVec2(0, height), true);
    ImGui::Columns(1, "order_columns");
    ImGui::Text("Sequence");
    ImGui::Separator();

    for (size_t i = 0; i < current_order.sequences.size(); ++i) {
        const int &item = current_order.sequences[i];

        ImGui::PushID(i);
        const bool incorrect = (item < 0 || item >= static_cast<int>(sequences.size()));
        const bool is_selected = (selected_sequence == i);
        const std::string order_string = std::to_string(item);
        if (is_selected) {
            ImGui::PushStyleColor(ImGuiCol_Text, GUI_HIGHLIGHT_COLOR);
        }

        if (ImGui::Selectable("##selectable", is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
            selected_sequence = i;
            digit_buffer.clear();
        }

        ImGui::SameLine();
        if (incorrect) {
            ImGui::TextColored(GUI_ERROR_COLOR, "%s", order_string.c_str());
        } else {
            ImGui::Text("%s", order_string.c_str());
        }

        if (is_selected) {
            ImGui::PopStyleColor();
        }

        ImGui::PopID();
    }

    ImGui::EndChild();
}

void GUIOrdersPanel::check_keyboard_input() {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows)) {
        return;
    }

    if (selected_sequence < 0 || selected_sequence >= current_order.sequences.size())
        return;

    for (int key = ImGuiKey_0; key <= ImGuiKey_9; key++) {
        if (ImGui::IsKeyPressed((ImGuiKey) key))
            digit_buffer.push_back('0' + (key - ImGuiKey_0));
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Backspace)) {
        if (!digit_buffer.empty()) {
            digit_buffer.pop_back();
        }
    }

    if (!digit_buffer.empty()) {
        try {
            int value = std::stoi(digit_buffer);
            value = std::max(std::min(value, static_cast<int>(sequences.size()) - 1), 0);
            current_order.sequences[selected_sequence] = value;
        } catch (std::out_of_range &) {
            digit_buffer.clear();
        }
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Enter)) {
        digit_buffer = std::to_string(current_order.sequences[selected_sequence]);
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) {
        selected_sequence = std::max(0, selected_sequence - 1);
        digit_buffer.clear();
    }

    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) {
        selected_sequence = std::min((int) current_order.sequences.size() - 1, selected_sequence + 1);
        digit_buffer.clear();
    }
}
