#include <algorithm>
#include <cmath>

#include "../utils/math.hpp"
#include "constants.hpp"
#include "frequencies.hpp"

FrequencyTable::FrequencyTable(const ScaleComposer &scale_composer, const double a4_frequency, const int notes)
    : scale_composer(scale_composer), notes(notes), a4_frequency(a4_frequency) {
}

void FrequencyTable::calculate(const double a4_freq) {
    a4_frequency = a4_freq;
    calculate_note_divisor();
    calculate_frequency_table();
    assign_note_names();
}

double FrequencyTable::get_note_frequency(uint8_t note_value) const {
    if (note_value >= notes) {
        return 0.0;
    }
    return frequencies[note_value];
}

std::string FrequencyTable::get_note_name(uint8_t note_value) const {
    if (note_value >= notes) {
        return "???";
    }
    return note_names[note_value];
}

int FrequencyTable::get_note_octave(uint8_t note_value) const {
    if (note_value >= notes) {
        return 0;
    }
    return note_octaves[note_value];
}

int FrequencyTable::get_note_value(const std::string &note_name, const int octave) const {
    auto it = note_values.find({note_name, octave});
    if (it != note_values.end()) {
        return it->second;
    }
    return -1;
}

int FrequencyTable::get_min_octave() const {
    return min_octave;
}

int FrequencyTable::get_max_octave() const {
    return max_octave;
}

int FrequencyTable::get_a4_index() const {
    return a4_index;
}

double FrequencyTable::get_a4_frequency() const {
    return a4_frequency;
}

std::vector<double> FrequencyTable::get_frequencies() const {
    return frequencies;
}

std::vector<std::string> FrequencyTable::get_note_names() const {
    return note_names;
}

std::map<std::pair<std::string, int>, size_t> FrequencyTable::get_note_values() const {
    return note_values;
}

double FrequencyTable::get_last_frequency() const {
    return frequencies.back();
}

void FrequencyTable::calculate_note_divisor() {
    const int edo = scale_composer.get_edo();
    note_divisor = std::pow(2.0, 1.0 / static_cast<double>(edo));
}

void FrequencyTable::calculate_frequency_table() {
    const int edo = scale_composer.get_edo();
    a4_index = notes / 2 - static_cast<int>(std::round(edo * std::log2(range_mean / a4_frequency)));
    double freq = a4_frequency * std::pow(note_divisor, static_cast<double>(notes - a4_index - 1));
    frequencies.resize(notes);
    for (int i = notes - 1; i >= 0; --i) {
        frequencies[i] = freq;
        freq /= note_divisor;
    }
}

void FrequencyTable::assign_note_names() {
    const int edo = scale_composer.get_edo();
    const auto scale = scale_composer.get_scale();
    const size_t a_index = scale_composer.get_a_index();
    note_octaves.clear();
    note_values.clear();
    note_names.clear();
    for (size_t i = 0; i < notes; ++i) {
        const int difference = i - a4_index;
        const int index = mod(difference + a_index, edo);

        const auto note_name = scale[index];
        const int octave = 4 + (difference + static_cast<int>(a_index)) / edo;
        note_names.push_back(note_name);
        note_octaves.push_back(octave);
        note_values[{note_name, octave}] = i;

        if (i == 0) {
            min_octave = octave;
        } else if (i == notes - 1) {
            max_octave = octave;
        }
    }
}
