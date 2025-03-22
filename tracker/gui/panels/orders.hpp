#ifndef GUI_PANELS_ORDERS_HPP
#define GUI_PANELS_ORDERS_HPP

#include <algorithm>
#include <stdio.h>
#include <string>
#include "../init.hpp"
#include "../names.hpp"
#include "utils.hpp"

class GUIOrdersPanel {
  private:
    struct CurrentOrder {
        int length;
        std::vector<int> sequences;
    } current_order;

    int order_index = 0;
    int selected_sequence = 0;
    const int max_items = 64;

    void from_order() {
        Order *order = orders[order_index];
        uint16_t total_length = order->order_length;
        current_order.sequences.resize(total_length);
        current_order.length = total_length;
        std::copy(order->sequences, order->sequences + total_length, current_order.sequences.begin());
    }

    void to_order() {
        Order *order = orders[order_index];
        const size_t length = current_order.length;
        const size_t structure_size = length + 1;

        Order *new_order = static_cast<Order *>(operator new(structure_size));
        new_order->order_length = length;
        for (size_t i = 0; i < length; ++i) {
            new_order->sequences[i] = current_order.sequences[i];
        }

        if (new_order->order_length > order->order_length) {
            for (size_t i = order->order_length; i < new_order->order_length; ++i) {
                new_order->sequences[i] = 0;
            }
        }

        orders[order_index] = new_order;
    }

    void update_orders() {
        update_items(order_names, orders.size(), "Order ", order_index);
    }

    void draw_order_length() {
        draw_number_of_items("Sequences", "##SequenceLength", current_order.length, 1, max_items);
    }

    void draw_order() {
        if (orders.empty()) {
            ImGui::Text("No orders available.");
            return;
        }

        ImGui::Separator();
        ImGui::Text("Order:");

        const float height = std::max(5.0f, ImGui::GetContentRegionAvail().y - 5.0f);
        ImGui::BeginChild("OrderScroll", ImVec2(0, height), true);
        ImGui::Columns(1, "order_columns");
        ImGui::Text("Sequence");
        ImGui::Separator();

        for (size_t i = 0; i < current_order.length; ++i) {
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

  public:
    GUIOrdersPanel() {
        update_orders();
    }

    void draw() {
        ImGui::Begin("Order Editor");
        ImGui::Columns(1, "order_columns");

        prepare_combo(order_names, "##OrderCombo", order_index);
        from_order();

        draw_order_length();
        draw_order();

        to_order();

        ImGui::Columns(1);
        ImGui::End();
    }
};

#endif // GUI_PANELS_ORDERS_HPP
