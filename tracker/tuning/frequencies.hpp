#ifndef TUNING_FREQUENCIES_HPP
#define TUNING_FREQUENCIES_HPP

#include "../constants.hpp"
#include "scale.hpp"

class FrequencyTable {
  public:
    FrequencyTable(const ScaleComposer &scale_composer, const double a4_frequency, const int notes = NOTES);

    std::vector<double> get_frequencies() const;
    std::vector<std::string> get_note_names() const;
    std::map<std::pair<std::string, int>, size_t> get_note_values() const;
    double get_last_frequency() const;

  private:
    const ScaleComposer &scale_composer;
    int edo;
    int notes;
    double a4_index;
    double a4_frequency;
    double note_divisor;

    std::vector<double> frequencies;
    std::vector<std::string> note_names;
    std::vector<int> note_octaves;
    std::map<std::pair<std::string, int>, size_t> note_values;

    void calculate_note_divisor();
    void calculate_frequency_table();
    void assign_note_names();
};

#endif // TUNING_FREQUENCIES_HPP
