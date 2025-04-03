#include <cmath>
#include <algorithm>
#include <sstream>

#include "constants.hpp"
#include "frequencies.hpp"

FrequencyTable::FrequencyTable(const ScaleComposer &scale_composer, const double a4_frequency, const int notes)
    : scale_composer(scale_composer), notes(notes), a4_frequency(a4_frequency) {
    edo = scale_composer.get_edo();
    calculate_note_divisor();
    calculate_frequency_table();
    assign_note_names();
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
    note_divisor = std::pow(2.0, 1.0 / static_cast<double>(edo));
}

void FrequencyTable::calculate_frequency_table() {
    a4_index = notes / 2 - static_cast<int>(std::round(edo * std::log2(range_mean / a4_frequency)));
    double freq = a4_frequency * std::pow(note_divisor, static_cast<double>(notes - a4_index - 1));
    frequencies.resize(notes);
    for (int i = notes - 1; i >= 0; --i) {
        frequencies[i] = freq;
        freq /= note_divisor;
    }
}

void FrequencyTable::assign_note_names() {
    const auto scale = scale_composer.get_scale();
    const size_t a_index = scale_composer.get_a_index();
    note_values.clear();
    note_names.clear();
    for (size_t i = 0; i < notes; ++i) {
        const int difference = i - a4_index;
        int index = (difference + a_index) % edo;
        if (index < 0) {
            index += edo;
        }

        const int octave = 4 + (difference + a_index) / edo;

        note_names.push_back(scale[index]);
        note_octaves.push_back(octave);
        note_values[{scale[index], octave}] = i;
    }
}