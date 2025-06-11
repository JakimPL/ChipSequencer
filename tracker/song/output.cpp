#include <cmath>
#include <cstring>

#include "../constants.hpp"
#include "../song/core.hpp"
#include "output.hpp"

uint8_t OutputType::set_output_flag(uint8_t &output_flag) const {
    output_flag = operation << 6;
    output_flag |= (variable_type << 4);
    output_flag |= shift;
    return output_flag;
}

uint8_t OutputType::set_item_flag(uint8_t &item_flag) const {
    if (splitter_on) {
        item_flag |= FLAG_SPLITTER;
    } else {
        item_flag &= ~FLAG_SPLITTER;
    }

    if (bypass) {
        item_flag |= FLAG_BYPASS;
    } else {
        item_flag &= ~FLAG_BYPASS;
    }

    return item_flag;
}

void OutputType::from_flags(const uint8_t output_flag, const uint8_t item_flag) {
    splitter_on = item_flag & FLAG_SPLITTER;
    bypass = item_flag & FLAG_BYPASS;

    operation = (output_flag & MASK_OPERATION) >> 6;
    variable_type = (output_flag & MASK_VARIABLE_TYPE) >> 4;
    shift = variable_type == 0 ? 0 : output_flag & MASK_SHIFT;
}

void OutputType::update_routing_item(const Target target) {
    routing_item = 0;
    if (routing_index < 0) {
        return;
    }

    if (target == Target::OSCILLATOR) {
        const Oscillator *oscillator = static_cast<Oscillator *>(oscillators[index]);
        const auto &routing = routing_variables.at(Target::OSCILLATOR);
        const auto [indices, _, __, ___] = routing.filter_items(oscillator->generator_index);

        for (size_t i = 0; i < indices.size(); ++i) {
            if (indices[i] == routing_index) {
                routing_item = static_cast<int>(i);
                break;
            }
        }
    } else if (target == Target::DSP) {
        const DSP *dsp = static_cast<DSP *>(dsps[index]);
        const auto &routing = routing_variables.at(Target::DSP);
        const auto [indices, _, __, ___] = routing.filter_items(dsp->effect_index);

        for (size_t i = 0; i < indices.size(); ++i) {
            if (indices[i] == routing_index) {
                routing_item = static_cast<int>(i);
                break;
            }
        }
    } else {
        routing_item = routing_index;
    }
}

void OutputType::from_link(const Link &link) {
    target = static_cast<int>(link.target);
    const int max_parameter = static_cast<int>(OutputTarget::Parameter);
    OutputTarget output_target = static_cast<OutputTarget>(std::min(max_parameter, target));

    output_channel = 0;
    dsp_channel = 0;
    splitter_on = false;
    switch (output_target) {
    case OutputTarget::OutputSplitter: {
        output_channel = link.index;
        splitter_on = true;
        break;
    }
    case OutputTarget::DSPSplitter: {
        dsp_channel = link.index;
        splitter_on = true;
        break;
    }
    case OutputTarget::DirectOutput: {
        output_channel = link.index;
        break;
    }
    case OutputTarget::DirectDSP: {
        dsp_channel = link.index;
        break;
    }
    case OutputTarget::Parameter: {
        const int target_offset = max_parameter;
        parameter_type = target - target_offset;
        target = target_offset;
        const auto &routing = routing_variables.at(link.target);
        index = link.index;
        offset = link.offset;
        routing_index = routing.get_index_from_offset(link.key);
        update_routing_item(link.target);
        break;
    }
    }
}

void OutputType::set_link(Link &link, const ItemType type, const uint8_t id) const {
    link.type = type;
    link.id = id;
    OutputTarget label = static_cast<OutputTarget>(target);
    switch (label) {
    case OutputTarget::DirectOutput:
    case OutputTarget::OutputSplitter: {
        link.target = static_cast<Target>(target);
        link.index = output_channel;
        link.offset = sizeof(_Float32) * output_channel;
        break;
    }
    case OutputTarget::DirectDSP:
    case OutputTarget::DSPSplitter: {
        link.target = static_cast<Target>(target);
        link.index = dsp_channel;
        link.offset = sizeof(_Float32) * dsp_channel;
        break;
    }
    case OutputTarget::Parameter:
    default: {
        link.target = static_cast<Target>(parameter_type + static_cast<int>(OutputTarget::Parameter));
        link.index = index;
        link.offset = offset;
        break;
    }
    }
}

void OutputType::load_splitter(const uint8_t target[]) {
    for (size_t i = 0; i < MAX_OUTPUT_CHANNELS; ++i) {
        const uint8_t value = target[i];
        splitter[i] = static_cast<float>(value) / UINT8_MAX;
    }
}

void OutputType::set_splitter(uint8_t target[]) const {
    for (size_t i = 0; i < MAX_OUTPUT_CHANNELS; ++i) {
        const float value = splitter[i];
        target[i] = static_cast<uint8_t>(std::round(value * UINT8_MAX));
    }
}
