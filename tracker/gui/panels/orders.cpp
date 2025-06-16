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

    if (select_item()) {
        from();
        draw_order();
        check_keyboard_input();
        to();
        history_actions();
        post_actions();
    } else {
        empty();
    }

    ImGui::End();
}

bool GUIOrdersPanel::select_item() {
    std::vector<std::string> dependencies = song.find_order_dependencies(order_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies);
    if (prepare_combo(this, order_names, "##OrderCombo", order_index).value_changed) {
        input_handler.clear();
    }
    show_dependency_tooltip(dependencies);
    pop_tertiary_style();
    ImGui::Separator();

    return !orders.empty();
}

void GUIOrdersPanel::empty() {
    ImGui::Text("No order available.");
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
    if (!save &&
        (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) ||
         !is_index_valid())) {
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
    input_handler.clear();
    update();
}

void GUIOrdersPanel::duplicate() {
    Order *new_order = song.duplicate_order(order_index);
    if (new_order == nullptr) {
        return;
    }

    order_index = orders.size() - 1;
    input_handler.clear();
    update();
}

void GUIOrdersPanel::remove() {
    const size_t previous_index = order_index;
    if (is_index_valid()) {
        song.remove_order(order_index);
        order_index = std::max(0, order_index - 1);
        update();
    }

    if (previous_index != order_index) {
        input_handler.clear();
    }
}

void GUIOrdersPanel::update() {
    update_items(order_names, orders.size(), "Order ", order_index);
    gui.update(GUIElement::Channels);
}

void GUIOrdersPanel::draw_order_length() {
    const size_t old_size = current_order.length;
    const LinkKey key = {Target::ORDER, order_index, ORDER_LENGTH};
    draw_number_of_items(this, "Sequences", "##SequenceLength", current_order.length, 1, MAX_ORDER_ITEMS, key);

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
        const size_t size = std::max(sequences.size(), commands_sequences.size());
        const bool incorrect = (item < 0 || item >= static_cast<int>(size));
        const bool is_selected = (selected_sequence == i);
        const std::string order_string = std::to_string(item);
        if (is_selected) {
            ImGui::PushStyleColor(ImGuiCol_Text, GUI_ROW_TEXT_CURRENT);
        }

        if (ImGui::Selectable("##selectable", is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
            selected_sequence = i;
            input_handler.clear(false);
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

    const int old_sequence_index = selected_sequence;
    const int old_sequence = current_order.sequences[selected_sequence];
    const size_t size = std::max(sequences.size(), commands_sequences.size());
    input_handler.set_limit(static_cast<int>(size) - 1);
    input_handler.handle_input(current_order.sequences, selected_sequence);
    if (old_sequence_index == selected_sequence) {
        const LinkKey key = {Target::ORDER, order_index, static_cast<uint16_t>(ORDER_SEQUENCES + selected_sequence)};
        perform_action_order_sequence(
            this, key, selected_sequence, old_sequence, current_order.sequences[selected_sequence]
        );
    }
}

void GUIOrdersPanel::set_index(const int index) {
    order_index = clamp_index(index, orders.size());
}

void GUIOrdersPanel::set_sequence(const size_t sequence_index, const size_t new_sequence) {
    if (sequence_index >= current_order.sequences.size()) {
        return;
    }

    current_order.sequences[sequence_index] = new_sequence;
    input_handler.clear(true);
}
