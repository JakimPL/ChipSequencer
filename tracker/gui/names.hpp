#ifndef GUI_NAMES_HPP
#define GUI_NAMES_HPP

#include <string>
#include <vector>

static std::vector<std::string> channel_names;
static std::vector<std::string> dsp_names;
static std::vector<std::string> effect_names;
static std::vector<std::string> envelope_names;
static std::vector<std::string> generator_names;
static std::vector<std::string> oscillator_names;
static std::vector<std::string> order_names;
static std::vector<std::string> sequence_names;
static std::vector<std::string> wavetable_names;

const std::vector<std::string> target_types = {"Output", "DSP", "Parameter"};
const std::vector<std::string> variable_types = {"Float", "Int8", "Int16", "Int32"};

#endif // GUI_NAMES_HPP
