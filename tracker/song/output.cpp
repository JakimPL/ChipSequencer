#include <cmath>

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
    OutputTarget output_target = static_cast<OutputTarget>(std::min(0, target));
    switch (output_target) {
    case OutputTarget::Splitter:
    case OutputTarget::DirectOutput:
        output_channel = link.index;
        break;
    case OutputTarget::DSP:
        dsp_channel = link.index;
        break;
    case OutputTarget::Parameter:
        const int target_offset = static_cast<int>(OutputTarget::Parameter);
        parameter_type = target - target_offset;
        target = target_offset;
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
    OutputTarget label = static_cast<OutputTarget>(target);
    switch (label) {
    case OutputTarget::Splitter:
        link.target = Target::SPLITTER;
        link.index = output_channel;
        link.offset = sizeof(_Float32) * output_channel;
        break;
    case OutputTarget::DirectOutput:
        link.target = Target::OUTPUT_CHANNEL;
        link.index = output_channel;
        link.offset = sizeof(_Float32) * output_channel;
        break;
    case OutputTarget::DSP:
        link.target = Target::DSP_CHANNEL;
        link.index = dsp_channel;
        link.offset = sizeof(_Float32) * dsp_channel;
        break;
    case OutputTarget::Parameter:
        link.target = static_cast<Target>(parameter_type + static_cast<int>(OutputTarget::Parameter));
        link.index = index;
        link.offset = offset;
        break;
    }
}

void OutputType::load_splitter(const uint8_t target[]) {
    splitter_on = false;
    for (size_t i = 0; i < MAX_OUTPUT_CHANNELS; ++i) {
        if (splitter[i] > 0) {
            splitter_on = true;
        }

        splitter[i] = static_cast<float>(target[i]) / UINT8_MAX;
    }
}

void OutputType::set_splitter(uint8_t target[]) const {
    for (size_t i = 0; i < MAX_OUTPUT_CHANNELS; ++i) {
        target[i] = splitter_on ? static_cast<uint8_t>(std::round(splitter[i] * UINT8_MAX)) : 0;
    }
}
