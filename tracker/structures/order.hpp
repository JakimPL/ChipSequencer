#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

using OrderSequences = std::array<uint8_t, UINT8_MAX>;

struct Order {
    uint8_t order_length;
    OrderSequences sequences;

    void serialize(std::ofstream &file) const;
    static Order *deserialize(std::ifstream &file);
} __attribute__((packed));

using Orders = std::vector<Order *>;

constexpr size_t ORDER_LENGTH = offsetof(Order, order_length);
constexpr size_t ORDER_SEQUENCES = offsetof(Order, sequences);
