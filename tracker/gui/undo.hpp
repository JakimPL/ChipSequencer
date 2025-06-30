#pragma once

#include <cstdint>
#include <string>

#include "../song/links/key.hpp"
#include "history/actions/selection.hpp"
#include "panels/panel.hpp"
#include "patterns/commands.hpp"
#include "patterns/selection.hpp"

template <typename T>
void perform_action(
    GUIPanel *owner,
    const LinkKey key,
    T &reference,
    const T old_value
);

void perform_action_lock(
    GUIPanel *owner,
    const LinkKey key,
    const bool &reference
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

void perform_action_note(
    GUIPanel *owner,
    const LinkKey key,
    const PatternRow pattern_row,
    const uint8_t old_note,
    const uint8_t new_note
);

void perform_action_command(
    GUIPanel *owner,
    const LinkKey key,
    const PatternRow pattern_row,
    const CommandValue old_command,
    const CommandValue new_command
);

void perform_action_add(
    GUIPanel *owner,
    const LinkKey key
);

template <typename T>
void perform_action_remove(
    GUIPanel *owner,
    const LinkKey key,
    const T *pointer
);

void perform_action_wavetable(
    GUIPanel *owner,
    const LinkKey key,
    int &wavetable_size,
    std::vector<float> &wavetable,
    const std::vector<float> &old_wavetable
);

template <typename T>
void perform_action_pattern_selection(
    GUIPanel *owner,
    const LinkKey key,
    const std::string &label,
    const PatternSelectionChange<T> &changes,
    const SetItemsFunction<T> function
);
