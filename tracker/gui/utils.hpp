#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../song/links/key.hpp"
#include "init.hpp"
#include "enums.hpp"
#include "keys.hpp"
#include "panels/panel.hpp"
#include "patterns/commands.hpp"
#include "patterns/display.hpp"
#include "patterns/pattern.hpp"
#include "patterns/selection.hpp"
#include "shortcuts/manager.hpp"

struct GUIState {
    bool value_changed;
    bool right_clicked;
};

enum class GUIAction : uint8_t {
    None,
    OK,
    Cancel,
};

extern std::unordered_map<LinkKey, bool> slider_states;
extern std::unordered_map<LinkKey, float> slider_float_values;

int clamp_index(int index, int size);
bool draw_number_of_items(const char *label_id, int &value, int min, int max, float label_length = 50.0f);

template <size_t n>
void draw_text(GUIPanel *owner, const char *label, char (&text)[n], LinkKey key);
bool draw_checkbox(GUIPanel *owner, const char *label, bool &reference, LinkKey key);
bool draw_int_slider(GUIPanel *owner, const char *label, int &reference, LinkKey key, int min = 0, int max = 1);
bool draw_float_slider(GUIPanel *owner, const char *label, float &reference, LinkKey key, float min = 0.0f, float max = 1.0f, GUIScale scale = GUIScale::Linear, const char *format = "%.4f");
bool draw_knob(GUIPanel *owner, const char *label, float &reference, LinkKey key, float min = 0.0f, float max = 1.0f);
void draw_link_tooltip(const LinkKey &key);

GUIAction draw_dialog_box_bottom();
bool draw_button(const char *label, float button_padding = 0.0f);
void draw_popup(const std::string &message);
void draw_confirmation_popup(
    const std::string &message,
    std::function<void()> ok_action = nullptr,
    std::function<void()> save_action = nullptr
);

void draw_output_output_splitter(GUIPanel *owner, OutputType &output_type, LinkKey key);
void draw_output_dsp_splitter(GUIPanel *owner, OutputType &output_type, int dsp_index, LinkKey key);
void draw_output_direct_output(GUIPanel *owner, OutputType &output_type, LinkKey key);
void draw_output_direct_dsp(GUIPanel *owner, OutputType &output_type, int dsp_index);
bool draw_output_parameter(GUIPanel *owner, OutputType &output_type);
void draw_output_parameter_generic(GUIPanel *owner, OutputType &output_type, const std::vector<std::string> &names, const std::string &label);
void draw_output_parameter_oscillator(GUIPanel *owner, OutputType &output_type);
void draw_output_parameter_dsp(GUIPanel *owner, OutputType &output_type);
bool draw_output(GUIPanel *owner, OutputType &output_type, LinkKey key);

void show_dependency_tooltip(std::vector<std::string> &dependencies);
void show_commands_pattern_tooltip(const CommandsPattern &pattern, size_t index);

bool draw_menu_item(const std::string &name, std::optional<ShortcutAction> action = std::nullopt, bool checked = false, bool condition = true);
bool get_menu_item(const std::string &name, std::optional<ShortcutAction> action = std::nullopt, bool checked = false);

std::pair<size_t, bool> draw_pattern(
    Pattern &pattern,
    PatternSelection &selection,
    SequenceRows &selected_rows,
    bool pattern_view,
    bool active_command,
    int edited_row = -1,
    size_t channel_index = 0,
    bool header = true,
    size_t index = 0,
    int playing_row = -1,
    uint16_t start = 0,
    uint16_t end = UINT16_MAX,
    bool locked = false
);
std::pair<size_t, bool> draw_commands_pattern(
    CommandsPattern &pattern,
    PatternSelection &selection,
    SequenceRows &selected_rows,
    bool pattern_view,
    bool active_command,
    int edited_row = -1,
    size_t channel_index = 0,
    bool header = true,
    size_t index = 0,
    int playing_row = -1,
    uint16_t start = 0,
    uint16_t end = UINT16_MAX,
    bool locked = false
);

GUIState prepare_combo(
    GUIPanel *owner,
    const std::vector<std::string> &names,
    const std::string &label,
    int &index,
    LinkKey key = {},
    bool error_if_empty = false,
    float margin_right = 0.0f
);

void update_items(std::vector<std::string> &names, size_t size, const std::string &label, int &index);
void push_secondary_style();
void pop_secondary_style();
void push_tertiary_style();
void pop_tertiary_style();

std::string get_row_id(size_t channel_index, int row);
std::string get_displayed_row(
    int row,
    int absolute_row,
    RowDisplayStyle row_display
);
