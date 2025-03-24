#include <fstream>

#include "../utils.hpp"
#include "channel.hpp"

void Channel::serialize(std::ofstream &file) const {
    uint16_t null = 0;
    write_data(file, &envelope_index, sizeof(uint8_t));
    write_data(file, &order_index, sizeof(uint8_t));
    write_data(file, &oscillator_index, sizeof(uint8_t));
    write_data(file, &null, sizeof(uint16_t));
    write_data(file, &pitch, sizeof(uint32_t));
    write_data(file, &output_flag, sizeof(uint8_t));
}
