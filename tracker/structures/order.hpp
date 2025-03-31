#ifndef STRUCTURES_ORDER_HPP
#define STRUCTURES_ORDER_HPP

#include <cstdint>
#include <fstream>
#include <vector>

struct Order {
    uint8_t order_length;
    uint8_t sequences[];

    void serialize(std::ofstream &file) const;
    static Order *deserialize(std::ifstream &file);
};

typedef std::vector<Order *> Orders;

#endif // STRUCTURES_ORDER_HPP
