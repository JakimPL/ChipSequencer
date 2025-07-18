#include "../../general.hpp"
#include "../../song/buffers.hpp"
#include "../../song/song.hpp"
#include "../../song/lock/registry.hpp"
#include "../enums.hpp"
#include "../gui.hpp"
#include "../names.hpp"
#include "../undo.hpp"
#include "../utils.hpp"
#include "../themes/theme.hpp"
#include "orders.hpp"

GUIOrdersPanel::GUIOrdersPanel(const bool visible, const bool windowed)
    : GUIPanel("Orders", visible, windowed) {
}

GUIElement GUIOrdersPanel::get_element() const {
    return GUIElement::Orders;
}

void GUIOrdersPanel::draw() {
    ImGui::BeginDisabled(lock_registry.is_locked(Target::ORDER, order_index));
    draw_order();
    ImGui::EndDisabled();
}

bool GUIOrdersPanel::select_item() {
    std::vector<std::string> dependencies = Song::find_order_dependencies(order_index);
    push_tertiary_style();
    draw_add_or_remove(dependencies);
    if (prepare_combo(this, order_names, "##OrderCombo", order_index, {}, false, GUI_COMBO_MARGIN_RIGHT).value_changed) {
        input_handler.clear();
    }
    show_dependency_tooltip(dependencies);
    lock_item(Target::ORDER, order_index);
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
    if (!is_index_valid()) {
        return;
    }

    if (!save && !focus) {
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
    Order *new_order = Song::add_order();
    if (new_order == nullptr) {
        return;
    }

    order_index = orders.size() - 1;
    input_handler.clear();
    perform_action_add(this, {Target::ORDER, order_index, 0});
    update();
}

void GUIOrdersPanel::duplicate() {
    Order *new_order = Song::duplicate_order(order_index);
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
        perform_action_remove(this, {Target::ORDER, order_index, 0}, orders[order_index]);
        Song::remove_order(order_index);
        order_index = std::max(0, order_index - 1);
        update();
    }

    if (previous_index != order_index) {
        input_handler.clear();
    }
}

void GUIOrdersPanel::update() {
    order_index = clamp_index(order_index, orders.size());
    update_items(order_names, orders.size(), "Order ", order_index);
    gui.update(GUIElement::Channels);
}

void GUIOrdersPanel::draw_order_length() {
    const size_t old_size = current_order.length;
    const LinkKey key = {Target::ORDER, order_index, ORDER_LENGTH};
    draw_number_of_items(this, "##SequenceLength", current_order.length, 1, MAX_ORDER_ITEMS, key);

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
    ImGui::BeginChild("OrderScroll", ImVec2(0, height), 1);
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
            ImGui::PushStyleColor(ImGuiCol_Text, theme.get_u32_color(ThemeItem::RowTextCurrent));
        }

        if (ImGui::Selectable("##selectable", is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
            selected_sequence = i;
            input_handler.clear(false);
        }

        ImGui::SameLine();
        if (incorrect) {
            ImGui::TextColored(theme.get_vec4_color(ThemeItem::Error), "%s", order_string.c_str());
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
    if (!focus) {
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
