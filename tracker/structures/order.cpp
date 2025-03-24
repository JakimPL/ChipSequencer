#include "../utils.hpp"
#include "order.hpp"

void Order::serialize(std::ofstream &file) const {
    write_data(file, &order_length, 1);
    for (size_t i = 0; i < order_length; i++) {
        write_data(file, &sequences[i], sizeof(uint8_t));
    }
}
