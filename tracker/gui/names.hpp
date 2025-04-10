#ifndef GUI_NAMES_HPP
#define GUI_NAMES_HPP

#include <string>
#include <vector>

extern std::vector<std::string> channel_names;
extern std::vector<std::string> dsp_names;
extern std::vector<std::string> envelope_names;
extern std::vector<std::string> oscillator_names;
extern std::vector<std::string> order_names;
extern std::vector<std::string> sequence_names;
extern std::vector<std::string> wavetable_names;

const std::vector<std::string> effect_names = {"Gainer", "Delay", "Filter"};
const std::vector<std::string> parameter_types = {"Envelope", "Sequence", "Oscillator", "Wavetable", "DSP", "Channel"};
const std::vector<std::string> target_types = {"Output", "DSP", "Parameter"};
const std::vector<std::string> variable_types = {"Float", "Int8", "Int16", "Int32"};
const std::vector<std::string> generator_names = {"Square", "Saw", "Sine", "Wavetable"};

#endif // GUI_NAMES_HPP
