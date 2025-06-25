#pragma once

#include <cstdint>

#include "../constants.hpp"
#include "../maps/routing.hpp"
#include "links/link.hpp"

enum class OutputTarget {
    OutputSplitter,
    DSPSplitter,
    DirectOutput,
    DirectDSP,
    Parameter,
};

enum class OutputOperation {
    Add = 0b00,
    Set = 0b01,
    Multiply = 0b10,
    Zero = 0b11,
};

struct OutputType {
    bool bypass = false;

    int operation = static_cast<int>(OutputOperation::Add);
    int shift = 0;
    int variable_type;
    int target;

    int output_channel;
    int dsp_channel;

    int parameter_type;
    int routing_item;
    int routing_index;

    int index;
    int offset;

    bool splitter_on = true;
    std::array<float, MAX_OUTPUT_CHANNELS> splitter = {
        static_cast<float>(DEFAULT_SPLITTER_0) / UINT8_MAX,
        static_cast<float>(DEFAULT_SPLITTER_1) / UINT8_MAX,
        static_cast<float>(DEFAULT_SPLITTER_2) / UINT8_MAX,
        static_cast<float>(DEFAULT_SPLITTER_3) / UINT8_MAX,
    };

    uint8_t set_output_flag(uint8_t &output_flag) const;
    uint8_t set_item_flag(uint8_t &item_flag) const;

    void update_routing_item(const Target target);
    void from_flags(const uint8_t output_flag, const uint8_t item_flag);
    void from_link(const Link &link);
    void set_link(Link &link, const ItemType type, const uint8_t id) const;

    void load_splitter(const uint8_t target[]);
    void set_splitter(uint8_t target[]) const;
};

static_assert(static_cast<uint8_t>(OutputTarget::OutputSplitter) == static_cast<uint8_t>(Target::SPLITTER_OUTPUT));
static_assert(static_cast<uint8_t>(OutputTarget::DSPSplitter) == static_cast<uint8_t>(Target::SPLITTER_DSP));
static_assert(static_cast<uint8_t>(OutputTarget::DirectOutput) == static_cast<uint8_t>(Target::DIRECT_OUTPUT));
static_assert(static_cast<uint8_t>(OutputTarget::DirectDSP) == static_cast<uint8_t>(Target::DIRECT_DSP));
static_assert(static_cast<uint8_t>(OutputTarget::Parameter) == static_cast<uint8_t>(Target::ENVELOPE));
