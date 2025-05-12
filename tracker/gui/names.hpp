#ifndef GUI_NAMES_HPP
#define GUI_NAMES_HPP

#include <map>
#include <string>
#include <vector>

#include "../song/links/target.hpp"
#include "../song/links/type.hpp"
#include "../structures/commands/sequence.hpp"
#include "enums.hpp"

extern std::vector<std::string> channel_names;
extern std::vector<std::string> dsp_names;
extern std::vector<std::string> envelope_names;
extern std::vector<std::string> oscillator_names;
extern std::vector<std::string> order_names;
extern std::vector<std::string> sequence_names;
extern std::vector<std::string> wavetable_names;
extern std::vector<std::string> commands_channel_names;
extern std::vector<std::string> commands_sequence_names;

const std::vector<std::string> effect_names = {"Gainer", "Distortion", "Filter", "Delay"};
const std::vector<std::string> parameter_types = {"Envelope", "Sequence", "Commands sequence", "Order", "Oscillator", "Wavetable", "DSP", "Channel", "Commands channel"};
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
    {Target::COMMANDS_SEQUENCE, "Commands sequence"},
    {Target::ORDER, "Order"},
    {Target::OSCILLATOR, "Oscillator"},
    {Target::WAVETABLE, "Wavetable"},
    {Target::DSP, "DSP"},
    {Target::CHANNEL, "Channel"},
    {Target::COMMANDS_CHANNEL, "Commands channel"},
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
    {GUIElement::CommandsChannel, "Commands channel"},
    {GUIElement::CommandsSequence, "Commands sequence"},
    {GUIElement::DSPs, "DSPs"},
    {GUIElement::Envelopes, "Envelopes"},
    {GUIElement::Orders, "Orders"},
    {GUIElement::Oscillators, "Oscillators"},
    {GUIElement::Patterns, "Patterns"},
    {GUIElement::Routing, "Routing"},
    {GUIElement::Sequences, "Sequences"},
    {GUIElement::Summary, "Summary"},
    {GUIElement::Wavetables, "Wavetables"},
};
const std::map<Instruction, const char *> instruction_names = {
    {Instruction::Empty, "Empty"},
    {Instruction::PortamentoUp, "Portamento up"},
    {Instruction::PortamentoDown, "Portamento down"},
    {Instruction::SetMasterGainer, "Set master gainer"},
    {Instruction::SetBPM, "Set BPM"},
    {Instruction::SetDivision, "Set division"},
    {Instruction::ChangeByteValue, "Change byte value"},
    {Instruction::ChangeWordValue, "Change word value"},
    {Instruction::ChangeDwordValue, "Change dword value"},
    {Instruction::ChangeFloatValue, "Change float value"},
};

#endif // GUI_NAMES_HPP
