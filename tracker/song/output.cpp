#include "../constants.hpp"
#include "output.hpp"

uint8_t OutputType::calculate_output_flag() const {
    uint8_t output_flag = 0;
    output_flag = operation << 6;
    output_flag |= (variable_type << 4);
    output_flag |= shift;
    return output_flag;
}

void OutputType::from_output_flag(const uint8_t output_flag) {
    operation = (output_flag & MASK_OPERATION) >> 6;
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
        try {
            routing_index = routing.offset_to_index.at(link.offset);
            offset = link.offset;
        } catch (const std::out_of_range &exception) {
            if (!routing.offset_to_index.empty()) {
                const auto &it = routing.offset_to_index.begin();
                routing_index = it->first;
                offset = it->second;
            }
        }
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