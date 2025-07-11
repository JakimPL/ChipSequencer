#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "../song/links/target.hpp"
#include "../song/links/type.hpp"
#include "../structures/commands/sequence.hpp"
#include "enums.hpp"
#include "clipboard/clipboard.hpp"
#include "patterns/commands.hpp"
#include "patterns/display.hpp"

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
const std::vector<std::string> simple_operation_names = {"Add", "Set"};
const std::vector<std::string> operation_names = {"Add", "Set", "Multiply", "Zero"};
const std::vector<std::string> variable_types = {"Float", "Byte (int8)", "Word (int16)", "Dword (int32)"};
const std::vector<std::string> generator_names = {"Square", "Saw", "Sine", "Wavetable", "Noise"};
const std::vector<const char *> row_display_style_names = {"Page", "Absolute", "Relative"};

const std::map<Target, std::string> target_names = {
    {Target::SPLITTER_OUTPUT, "Output splitter"},
    {Target::SPLITTER_DSP, "DSP splitter"},
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
    {Target::SPECIAL, "Special"},
    {Target::COUNT, "Unused"},
};
const std::map<ItemType, std::string> item_types_names = {
    {ItemType::CHANNEL, "Channel"},
    {ItemType::DSP, "DSP"},
    {ItemType::COMMANDS, "Command"},
};
const std::vector<std::pair<GUIElement, const char *>> menu_items = {
    {GUIElement::Editor, "Editor"},
    {GUIElement::General, "General"},
    {GUIElement::Channels, "Channels"},
    {GUIElement::CommandsChannels, "Commands channel"},
    {GUIElement::CommandsSequences, "Commands sequence"},
    {GUIElement::DSPs, "DSPs"},
    {GUIElement::Envelopes, "Envelopes"},
    {GUIElement::Orders, "Orders"},
    {GUIElement::Oscillators, "Oscillators"},
    {GUIElement::Patterns, "Patterns"},
    {GUIElement::Routings, "Routing"},
    {GUIElement::Sequences, "Sequences"},
    {GUIElement::Summary, "Summary"},
    {GUIElement::Waveform, "Waveform"},
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
    {Instruction::AddByteValue, "Add byte value"},
    {Instruction::AddWordValue, "Add word value"},
    {Instruction::AddDwordValue, "Add dword value"},
    {Instruction::AddFloatValue, "Add float value"},
};
const std::vector<std::string> simple_instruction_names = {
    "Empty",
    "Portamento up",
    "Portamento down",
    "Set master gainer",
    "Set BPM",
    "Set division",
    "Change value",
};
const std::map<ClipboardCategory, const char *> clipboard_category_names = {
    {ClipboardCategory::Notes, "Notes"},
    {ClipboardCategory::Commands, "Commands"},
    {ClipboardCategory::None, "None"},
};

std::string get_note_name(uint8_t note_value);
std::string get_note_octave(uint8_t note_value);
uint8_t get_note_value(const std::string &note_name, int octave);
std::string get_full_note_name(uint8_t note_value);
std::string get_command_name(const CommandValue &command_value);

std::vector<std::string> get_note_names();
