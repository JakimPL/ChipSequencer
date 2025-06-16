#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../song/links/key.hpp"
#include "init.hpp"
#include "enums.hpp"
#include "node.hpp"
#include "panels/panel.hpp"
#include "history/shortcuts.hpp"
#include "patterns/commands.hpp"
#include "patterns/pattern.hpp"
#include "patterns/selection.hpp"

struct GUIState {
    bool value_changed;
    bool right_clicked;
};

extern std::unordered_map<LinkKey, bool> slider_states;
extern std::unordered_map<LinkKey, float> slider_float_values;

int clamp_index(int index, const int size);
void draw_number_of_items(GUIPanel *owner, const std::string &label, const char *label_id, int &value, int min, int max, const LinkKey key = {}, float label_length = 50.0f);

template <size_t n>
void draw_text(GUIPanel *owner, const char *label, char (&text)[n], const LinkKey key);
void draw_checkbox(GUIPanel *owner, const char *label, bool &reference, const LinkKey key);
void draw_int_slider(GUIPanel *owner, const char *label, int &reference, const LinkKey key, int min = 0, int max = 1);
void draw_float_slider(GUIPanel *owner, const char *label, float &reference, const LinkKey, float min = 0.0f, float max = 1.0f, const GUIScale scale = GUIScale::Linear, const char *format = "%.4f");
void draw_knob(GUIPanel *owner, const char *label, float &reference, const LinkKey key, float min = 0.0f, float max = 1.0f);
void draw_link_tooltip(const LinkKey &key);

bool draw_button(const char *label, const float button_padding = 0.0f);
void draw_popup(const std::string &message);
void draw_confirmation_popup(
    const std::string &message,
    std::function<void()> ok_action = nullptr,
    std::function<void()> save_action = nullptr
);

void draw_output_output_splitter(GUIPanel *owner, OutputType &output_type, const LinkKey key);
void draw_output_dsp_splitter(GUIPanel *owner, OutputType &output_type, const int dsp_index, const LinkKey key);
void draw_output_direct_output(GUIPanel *owner, OutputType &output_type, const LinkKey key);
void draw_output_direct_dsp(GUIPanel *owner, OutputType &output_type, const int dsp_index, const LinkKey key);
bool draw_output_parameter(GUIPanel *owner, OutputType &output_type, const LinkKey key);
void draw_output_parameter_generic(GUIPanel *owner, OutputType &output_type, const std::vector<std::string> &names, const std::string label);
void draw_output_parameter_oscillator(GUIPanel *owner, OutputType &output_type);
void draw_output_parameter_dsp(GUIPanel *owner, OutputType &output_type);
bool draw_output(GUIPanel *owner, OutputType &output_type, const LinkKey key);

void show_dependency_tooltip(std::vector<std::string> &dependencies);
void show_commands_pattern_tooltip(const CommandsPattern &pattern, const size_t index);

void draw_menu_item(const std::string &name, const std::optional<ShortcutAction> action = std::nullopt, const bool checked = false);
bool get_menu_item(const std::string &name, const std::optional<ShortcutAction> action = std::nullopt, const bool checked = false);

std::pair<size_t, bool> draw_pattern(
    Pattern &pattern,
    PatternSelection &selection,
    PatternRows &selected_rows,
    const bool pattern_view,
    const size_t channel_index = 0,
    const bool header = true,
    const size_t index = 0,
    const int playing_row = -1,
    const uint16_t start = 0,
    const uint16_t end = UINT16_MAX
);
std::pair<size_t, bool> draw_commands_pattern(
    CommandsPattern &pattern,
    PatternSelection &selection,
    PatternRows &selected_rows,
    const bool pattern_view,
    const size_t channel_index = 0,
    const bool header = true,
    const size_t index = 0,
    const int playing_row = -1,
    const uint16_t start = 0,
    const uint16_t end = UINT16_MAX
);

GUIState prepare_combo(GUIPanel *owner, const std::vector<std::string> &names, std::string label, int &index, const LinkKey = {}, const bool error_if_empty = false);
void update_items(std::vector<std::string> &names, size_t size, std::string label, int &index);
void push_secondary_style();
void pop_secondary_style();
void push_tertiary_style();
void pop_tertiary_style();

std::string get_note_name(uint8_t note_value);
std::string get_note_octave(uint8_t note_value);
uint8_t get_note_value(const std::string &note_name, const int octave);

template <typename T>
void perform_action(
    GUIPanel *owner,
    const LinkKey key,
    T &reference,
    const T old_value
);

void perform_action_float(
    GUIPanel *owner,
    const LinkKey key,
    float &reference,
    const float old_value,
    const char *format = "%.4f"
);

void perform_action_routing(
    GUIPanel *owner,
    const LinkKey key,
    const InputKey &input_key,
    const LinkKey &new_value,
    const LinkKey &old_value
);

void perform_action_order_sequence(
    GUIPanel *owner,
    const LinkKey key,
    const size_t sequence_index,
    const size_t old_sequence,
    const size_t new_sequence
);

template <size_t n>
void perform_action_string(
    GUIPanel *owner,
    const LinkKey key,
    char (&buffer)[n],
    const std::string &old_value
);
