#include "../constants.hpp"
#include "output.hpp"

uint8_t OutputType::calculate_output_flag() const {
    uint8_t output_flag = 0;
    output_flag = additive ? 0 : MASK_ADDITIVE;
    output_flag |= (variable_type << 4) | shift;
    return output_flag;
}

void OutputType::from_output_flag(const uint8_t output_flag) {
    additive = !(output_flag & MASK_ADDITIVE);
    variable_type = (output_flag & MASK_VARIABLE_TYPE) >> 4;
    shift = variable_type == 0 ? 0 : output_flag & MASK_SHIFT;
}
