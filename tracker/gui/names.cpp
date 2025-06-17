#include <sstream>

#include "../general.hpp"
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
    if (note_value == NOTE_REST) return "...";
    if (note_value == NOTE_OFF) return "===";
    if (note_value < NOTES) {
        return frequency_table.get_note_name(note_value);
    }
    return "???";
}

std::string get_note_octave(const uint8_t note_value) {
    if (note_value == NOTE_REST) return "...";
    if (note_value == NOTE_OFF) return "===";
    if (note_value < NOTES) {
        return std::to_string(frequency_table.get_note_octave(note_value));
    }
    return 0;
}

uint8_t get_note_value(const std::string &note_name, const int octave) {
    if (note_name == "...") return NOTE_REST;
    if (note_name == "===") return NOTE_OFF;
    if (note_name == "???") return NOTES;
    return frequency_table.get_note_value(note_name, octave);
}

std::string get_full_note_name(const uint8_t note) {
    std::ostringstream oss;
    switch (note) {
    case NOTE_REST: {
        oss << "rest";
        break;
    }
    case NOTE_OFF: {
        oss << "off";
        break;
    }
    case NOTES: {
        oss << "invalid";
        break;
    }
    default:
        oss << get_note_name(note) << get_note_octave(note);
        break;
    }

    return oss.str();
}
