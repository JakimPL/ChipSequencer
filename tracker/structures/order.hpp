#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

typedef std::array<uint8_t, UINT8_MAX> OrderSequences;

struct Order {
    uint8_t order_length;
    OrderSequences sequences;

    void serialize(std::ofstream &file) const;
    static Order *deserialize(std::ifstream &file);
} __attribute__((packed));

typedef std::vector<Order *> Orders;

constexpr size_t ORDER_LENGTH = offsetof(Order, order_length);
constexpr size_t ORDER_SEQUENCES = offsetof(Order, sequences);
