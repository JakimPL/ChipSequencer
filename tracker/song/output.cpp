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

void OutputType::from_link(const Link &link) {
    target = static_cast<int>(link.target);
    switch (target) {
    case OUTPUT_TARGET_OUTPUT:
        output_channel = link.index;
        break;
    case OUTPUT_TARGET_DSP:
        dsp_channel = link.index;
        break;
    default:
        parameter_type = target - OUTPUT_TARGET_PARAMETER;
        target = OUTPUT_TARGET_PARAMETER;
        const auto &routing = routing_variables.at(link.target);
        index = link.index;
        offset = link.offset;
        routing_item = routing.offset_to_index.at(link.offset);
        break;
    }
}

void OutputType::set_link(Link &link, const ItemType type, const uint8_t id) const {
    link.type = type;
    link.id = id;
    switch (target) {
    case OUTPUT_TARGET_OUTPUT:
        link.target = Target::OUTPUT_CHANNEL;
        link.index = output_channel;
        link.offset = 4 * output_channel;
        break;
    case OUTPUT_TARGET_DSP:
        link.target = Target::DSP_CHANNEL;
        link.index = dsp_channel;
        link.offset = 4 * dsp_channel;
        break;
    case OUTPUT_TARGET_PARAMETER:
        link.target = static_cast<Target>(parameter_type + OUTPUT_TARGET_PARAMETER);
        link.index = index;
        link.offset = offset;
        break;
    }
}