#ifndef GUI_UTILS_HPP
#define GUI_UTILS_HPP

#include <functional>
#include <string>
#include <vector>

#include "init.hpp"
#include "pattern.hpp"

int clamp_index(int index, const int max);
void draw_number_of_items(const std::string &label, const char *label_id, int &value, int min, int max, float label_length = 50.0f);
void draw_int_slider(const char *label, int &reference, int min, int max);
void draw_float_slider(const char *label, float &reference, float min, float max, bool log_scale = false);
void draw_knob(const char *label, float &reference, float min, float max);
void draw_popup(const std::string &message);
void draw_button(const char *label, const std::function<void()> &callback, const float button_padding = 0.0f);
size_t draw_pattern(Pattern &pattern, const bool header = true, size_t index = 0);
void prepare_combo(const std::vector<std::string> &names, std::string label, int &index);
void update_items(std::vector<std::string> &names, size_t size, std::string label, int &index);

std::string get_note_name(uint8_t note_value);
std::string get_note_octave(uint8_t note_value);
uint8_t get_note_value(const std::string &note_name, const int octave);

#endif // GUI_UTILS_HPP
