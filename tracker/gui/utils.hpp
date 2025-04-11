#ifndef GUI_UTILS_HPP
#define GUI_UTILS_HPP

#include <string>
#include <vector>

#include "init.hpp"
#include "pattern.hpp"

int clamp_index(int index, const int max);
void draw_number_of_items(const std::string &label, const char *label_id, int &value, int min, int max, float label_length = 50.0f);
void draw_int_slider(const char *label, int &reference, int min, int max);
void draw_float_slider(const char *label, float &reference, float min, float max, bool log_scale = false, const char *format = "%.4f");
void draw_knob(const char *label, float &reference, float min, float max);
void draw_popup(const std::string &message);
bool draw_button(const char *label, const float button_padding = 0.0f);

bool draw_output(OutputType &output_type);
void draw_output_parameter(OutputType &output_type, const std::vector<std::string> &names, const std::string label);
void draw_output_parameter_oscillator(OutputType &output_type);
void draw_output_parameter_dsp(OutputType &output_type);

std::pair<size_t, bool> draw_pattern(Pattern &pattern, const bool header = true, const size_t index = 0, const int playing_row = -1, const uint16_t start = 0, const uint16_t end = UINT16_MAX);
bool prepare_combo(const std::vector<std::string> &names, std::string label, int &index, const bool error_if_empty = false);
void update_items(std::vector<std::string> &names, size_t size, std::string label, int &index);
void push_secondary_style();
void pop_secondary_style();
void push_tertiary_style();
void pop_tertiary_style();

std::string get_note_name(uint8_t note_value);
std::string get_note_octave(uint8_t note_value);
uint8_t get_note_value(const std::string &note_name, const int octave);

#endif // GUI_UTILS_HPP
