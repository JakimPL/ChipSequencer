#ifndef GUI_NAMES_HPP
#define GUI_NAMES_HPP

#include <map>
#include <string>
#include <vector>

#include "../song/links/target.hpp"
#include "../song/links/type.hpp"
#include "enums.hpp"

extern std::vector<std::string> channel_names;
extern std::vector<std::string> dsp_names;
extern std::vector<std::string> envelope_names;
extern std::vector<std::string> oscillator_names;
extern std::vector<std::string> order_names;
extern std::vector<std::string> sequence_names;
extern std::vector<std::string> wavetable_names;

const std::vector<std::string> effect_names = {"Gainer", "Distortion", "Filter", "Delay"};
const std::vector<std::string> parameter_types = {"Envelope", "Sequence", "Order", "Oscillator", "Wavetable", "DSP", "Channel"};
const std::vector<std::string> target_types = {"Output splitter", "DSP splitter", "Direct output", "DSP", "Parameter"};
const std::vector<std::string> operation_names = {"Add", "Set", "Multiply", "Zero"};
const std::vector<std::string> variable_types = {"Float", "Int8", "Int16", "Int32"};
const std::vector<std::string> generator_names = {"Square", "Saw", "Sine", "Wavetable", "Noise"};
const std::map<Target, std::string> target_names = {
    {Target::SPLITTER_OUTPUT, "Output Splitter"},
    {Target::SPLITTER_DSP, "DSP Splitter"},
    {Target::DIRECT_OUTPUT, "Output"},
    {Target::DIRECT_DSP, "DSP"},
    {Target::ENVELOPE, "Envelope"},
    {Target::SEQUENCE, "Sequence"},
    {Target::ORDER, "Order"},
    {Target::OSCILLATOR, "Oscillator"},
    {Target::WAVETABLE, "Wavetable"},
    {Target::DSP, "DSP"},
    {Target::CHANNEL, "Channel"},
    {Target::UNUSED, "Unused"},
};
const std::map<ItemType, std::string> item_types_names = {
    {ItemType::CHANNEL, "Channel"},
    {ItemType::DSP, "DSP"},
};
const std::vector<std::pair<GUIElement, const char *>> menu_items = {
    {GUIElement::Editor, "Editor"},
    {GUIElement::General, "General"},
    {GUIElement::Channels, "Channels"},
    {GUIElement::DSPs, "DSPs"},
    {GUIElement::Envelopes, "Envelopes"},
    {GUIElement::Orders, "Orders"},
    {GUIElement::Oscillators, "Oscillators"},
    {GUIElement::Patterns, "Patterns"},
    {GUIElement::Routing, "Routing"},
    {GUIElement::Sequences, "Sequences"},
    {GUIElement::Wavetables, "Wavetables"},
};

#endif // GUI_NAMES_HPP
