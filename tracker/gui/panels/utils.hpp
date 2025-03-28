#ifndef GUI_PANELS_UTILS_HPP
#define GUI_PANELS_UTILS_HPP

#include <string>
#include <vector>

#include "../init.hpp"

#define NOTE_REST -2
#define NOTE_OFF -1
#define CONSTANT_PITCH 0xFF

extern const char *note_names[];

void draw_number_of_items(const std::string &label, const char *label_id, int &value, int min, int max, float label_length = 50.0f);
void draw_int_slider(const char *label, int &reference, int min, int max);
void draw_float_slider(const char *label, float &reference, float min, float max, bool log_scale = false);
void draw_knob(const char *label, float &reference, float min, float max);
void prepare_combo(const std::vector<std::string> &names, std::string label, int &index);
void update_items(std::vector<std::string> &names, size_t size, std::string label, int &index);
std::string get_note_name(int8_t midi_value);
uint8_t get_midi_value(const std::string &note_name);

#endif // GUI_PANELS_UTILS_HPP
