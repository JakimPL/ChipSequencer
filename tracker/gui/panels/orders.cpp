#include "../../general.hpp"
#include "../enums.hpp"
#include "../names.hpp"
#include "../utils.hpp"
#include "orders.hpp"

GUIOrdersPanel::GUIOrdersPanel(const bool visible)
    : GUIPanel(visible) {
    from();
    update();
}

void GUIOrdersPanel::draw() {
    ImGui::Begin("Orders");
    ImGui::Columns(1, "order_columns");

    std::vector<size_t> dependencies = song.find_order_dependencies(order_index);
    push_tertiary_style();
    draw_add_or_remove("channels", dependencies);
    prepare_combo(order_names, "##OrderCombo", order_index);
    show_dependency_tooltip("channels", dependencies);
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
    current_order.sequences.clear();
    current_order.length = total_length;
    for (size_t i = 0; i < total_length; ++i) {
        current_order.sequences.push_back(order->sequences[i]);
    }
}

void GUIOrdersPanel::to() const {
    if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || !is_index_valid()) {
        return;
    }

    Order *order = orders[order_index];
    const size_t length = current_order.length;
    order->order_length = length;
    for (size_t i = 0; i < length; ++i) {
        if (i < current_order.sequences.size()) {
            const uint8_t sequence = current_order.sequences[i];
            order->sequences[i] = sequence;
            buffers.orders[order_index][i] = sequence;
        } else {
            order->sequences[i] = buffers.orders[order_index][i];
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

void GUIOrdersPanel::duplicate() {
    Order *new_order = song.duplicate_order(order_index);
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
    const size_t old_size = current_order.length;
    draw_number_of_items("Sequences", "##SequenceLength", current_order.length, 1, MAX_ORDER_ITEMS);

    if (old_size != current_order.length) {
        current_order.sequences.resize(current_order.length);
        if (current_order.length > old_size) {
            for (size_t i = old_size; i < current_order.length; i++) {
                current_order.sequences[i] = buffers.orders[order_index][i];
            }
        }
    }
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

    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    if (selected_sequence < 0 || selected_sequence >= current_order.sequences.size()) {
        return;
    }

    for (int key = ImGuiKey_0; key <= ImGuiKey_9; key++) {
        if (ImGui::IsKeyPressed((ImGuiKey) key)) {
            digit_buffer.push_back('0' + (key - ImGuiKey_0));
        }
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

void GUIOrdersPanel::set_index(const int index) {
    order_index = clamp_index(index, orders.size());
}
