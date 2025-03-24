#include "../utils.hpp"
#include "dsp.hpp"

void DSPDelay::serialize(std::ofstream &file) const {
    uint16_t null = 0;
    write_data(file, &dsp_size, sizeof(uint8_t));
    write_data(file, &effect_index, sizeof(uint8_t));
    write_data(file, &null, sizeof(uint16_t));
    write_data(file, &output_flag, sizeof(uint8_t));
    write_data(file, &dry, sizeof(uint16_t));
    write_data(file, &wet, sizeof(uint16_t));
    write_data(file, &feedback, sizeof(uint16_t));
    write_data(file, &delay_time, sizeof(uint16_t));
}

void DSPGainer::serialize(std::ofstream &file) const {
    uint16_t null = 0;
    write_data(file, &dsp_size, sizeof(uint8_t));
    write_data(file, &effect_index, sizeof(uint8_t));
    write_data(file, &null, sizeof(uint16_t));
    write_data(file, &output_flag, sizeof(uint8_t));
    write_data(file, &volume, sizeof(uint16_t));
}

void DSPFilter::serialize(std::ofstream &file) const {
    uint16_t null = 0;
    write_data(file, &dsp_size, sizeof(uint8_t));
    write_data(file, &effect_index, sizeof(uint8_t));
    write_data(file, &null, sizeof(uint16_t));
    write_data(file, &output_flag, sizeof(uint8_t));
    write_data(file, &frequency, sizeof(uint16_t));
}
