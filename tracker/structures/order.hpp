#ifndef ORDER_HPP
#define ORDER_HPP

#include <cstdint>
#include <vector>

struct Order {
    uint8_t order_length;
    uint8_t sequences[];
};

#endif // ORDER_HPP
