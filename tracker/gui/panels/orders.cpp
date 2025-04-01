#include "../../general.hpp"
#include "orders.hpp"

GUIOrdersPanel::GUIOrdersPanel() {
    from();
    update();
}

void GUIOrdersPanel::draw() {
    ImGui::Begin("Order Editor");
    ImGui::Columns(1, "order_columns");

    draw_add_or_remove();
    prepare_combo(order_names, "##OrderCombo", order_index);
    ImGui::Separator();

    from();
    draw_order();
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

    Order *order = orders[order_index];
    uint16_t total_length = order->order_length;
    current_order.sequences.resize(total_length);
    current_order.length = total_length;
    std::copy(order->sequences, order->sequences + total_length, current_order.sequences.begin());
}

void GUIOrdersPanel::to() const {
    if (!is_index_valid()) {
        return;
    }

    Order *order = orders[order_index];
    const size_t length = current_order.length;
    const size_t structure_size = length + 1;

    Order *new_order = static_cast<Order *>(operator new(structure_size));
    new_order->order_length = length;
    for (size_t i = 0; i < length; ++i) {
        if (i < current_order.sequences.size()) {
            new_order->sequences[i] = current_order.sequences[i];
        } else {
            new_order->sequences[i] = 0;
        }
    }

    orders[order_index] = new_order;
    delete order;
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
    gui.channels_panel.update();
}

void GUIOrdersPanel::draw_order_length() {
    draw_number_of_items("Sequences", "##SequenceLength", current_order.length, 1, max_items);
}

void GUIOrdersPanel::draw_order() {
    if (!is_index_valid()) {
        ImGui::Text("No order available.");
        return;
    }

    ImGui::Separator();
    ImGui::Text("Order:");

    draw_order_length();

    const float height = std::max(5.0f, ImGui::GetContentRegionAvail().y - 5.0f);
    ImGui::BeginChild("OrderScroll", ImVec2(0, height), true);
    ImGui::Columns(1, "order_columns");
    ImGui::Text("Sequence");
    ImGui::Separator();

    for (size_t i = 0; i < current_order.sequences.size(); ++i) {
        int &item = current_order.sequences[i];

        ImGui::PushID(i);
        const bool is_selected = (selected_sequence == i);
        const std::string order_string = std::to_string(item);
        if (ImGui::Selectable("##selectable", is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
            selected_sequence = i;
        }

        ImGui::SameLine();
        if (is_selected) {
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 1.0f, 1.0f), "%s", order_string.c_str());
        } else {
            ImGui::Text("%s", order_string.c_str());
        }

        ImGui::PopID();
    }

    ImGui::EndChild();
}
