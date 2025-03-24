#ifndef STRUCTURES_ORDER_HPP
#define STRUCTURES_ORDER_HPP

#include <cstdint>
#include <vector>

struct Order {
    uint8_t order_length;
    uint8_t sequences[];

    void serialize(std::ofstream &file) const {
        write_data(file, &order_length, 1);
        for (size_t i = 0; i < order_length; i++) {
            write_data(file, &sequences[i], sizeof(uint8_t));
        }
    }
};

typedef std::vector<Order *> Orders;

#endif // STRUCTURES_ORDER_HPP
