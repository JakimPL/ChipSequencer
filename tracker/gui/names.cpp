#include <sstream>

#include "../general.hpp"
#include "../maps/commands.hpp"
#include "../tuning/frequencies.hpp"
#include "names.hpp"

std::vector<std::string> channel_names;
std::vector<std::string> dsp_names;
std::vector<std::string> envelope_names;
std::vector<std::string> oscillator_names;
std::vector<std::string> order_names;
std::vector<std::string> sequence_names;
std::vector<std::string> wavetable_names;
std::vector<std::string> commands_channel_names;
std::vector<std::string> commands_sequence_names;

std::string get_note_name(const uint8_t note_value) {
    switch (note_value) {
    case NOTE_CUT: {
        return "---";
    }
    case NOTE_REST: {
        return "...";
    }
    case NOTE_OFF: {
        return "===";
    }
    default:
        if (note_value < NOTES) {
            return frequency_table.get_note_name(note_value);
        }
    }

    return "???";
}

std::string get_note_octave(const uint8_t note_value) {
    switch (note_value) {
    case NOTE_CUT: {
        return "---";
    }
    case NOTE_REST: {
        return "...";
    }
    case NOTE_OFF: {
        return "===";
    }
    default: {
        if (note_value < NOTES) {
            return std::to_string(frequency_table.get_note_octave(note_value));
        }
    }
    }

    throw std::runtime_error("Invalid note value: " + std::to_string(note_value));
}

uint8_t get_note_value(const std::string &note_name, const int octave) {
    if (note_name == "...") return NOTE_REST;
    if (note_name == "---") return NOTE_CUT;
    if (note_name == "===") return NOTE_OFF;
    if (note_name == "???") return NOTES;
    return frequency_table.get_note_value(note_name, octave);
}

std::string get_full_note_name(const uint8_t note) {
    std::ostringstream oss;
    switch (note) {
    case NOTE_CUT: {
        oss << "Cut";
        break;
    }
    case NOTE_REST: {
        oss << "Rest";
        break;
    }
    case NOTE_OFF: {
        oss << "Off";
        break;
    }
    case NOTES: {
        oss << "Invalid";
        break;
    }
    default:
        oss << get_note_name(note) << get_note_octave(note);
        break;
    }

    return oss.str();
}

std::string get_command_name(const CommandValue &command_value) {
    if (command_value.first.empty()) {
        return "Empty";
    }

    const char command_character = command_value.first[0];
    const Instruction instruction = command_characters.at(command_character);
    return instruction_names.at(instruction);
}

std::vector<std::string> get_note_names() {
    std::vector<std::string> note_names;
    for (int note = NOTE_CUT; note <= NOTE_OFF; ++note) {
        note_names.push_back(get_full_note_name(note));
    }

    for (uint8_t note = 0; note < NOTES; ++note) {
        note_names.push_back(get_full_note_name(note));
    }

    return note_names;
}
