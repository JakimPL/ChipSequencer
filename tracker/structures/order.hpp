#ifndef STRUCTURES_ORDER_HPP
#define STRUCTURES_ORDER_HPP

#include <array>
#include <cstdint>
#include <fstream>
#include <vector>

typedef std::array<uint8_t, UINT8_MAX> OrderSequences;

struct Order {
    uint8_t order_length;
    OrderSequences sequences;

    void serialize(std::ofstream &file) const;
    static Order *deserialize(std::ifstream &file);
};

typedef std::vector<Order *> Orders;

#endif // STRUCTURES_ORDER_HPP
