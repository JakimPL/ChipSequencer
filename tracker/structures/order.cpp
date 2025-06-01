#include "../general.hpp"
#include "../utils/file.hpp"
#include "order.hpp"

void Order::serialize(std::ofstream &file) const {
    write_data(file, &order_length, 1);
    for (size_t i = 0; i < order_length; i++) {
        write_data(file, &sequences[i], sizeof(uint8_t));
    }
}

Order *Order::deserialize(std::ifstream &file) {
    if (!file.good()) {
        throw std::runtime_error("Order file stream is not good");
    }

    uint8_t length;
    read_data(file, &length, sizeof(length));

    Order *order = resource_manager.allocate<Order>();
    order->order_length = length;

    for (size_t i = 0; i < length; i++) {
        read_data(file, &order->sequences[i], sizeof(uint8_t));
    }

    return order;
}
