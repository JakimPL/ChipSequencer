#ifndef TUNING_SCALE_HPP
#define TUNING_SCALE_HPP

#include <string>
#include <vector>
#include <map>

#include "constants.hpp"

class ScaleComposer {
  public:
    ScaleComposer();
    void compose(const int new_edo);
    int get_edo() const;
    size_t get_a_index() const;
    std::vector<std::string> get_scale() const;

  private:
    std::string render(const std::string &name, const int offset) const;
    void get_note_centers_and_limits();

    int edo = 0;
    int a_index = -1;
    std::vector<double> centers;
    std::vector<double> limits;
    std::vector<std::string> scale;

    const std::vector<std::string> scale_names = {
        "C",
        "D",
        "E",
        "F",
        "G",
        "A",
        "B",
    };

    const std::vector<double> scale_intervals = {
        1.0,
        9.0 / 8.0,
        5.0 / 4.0,
        4.0 / 3.0,
        3.0 / 2.0,
        5.0 / 3.0,
        15.0 / 8.0,
        2.0,
    };

    const std::map<int, std::string> scale_symbols = {
        {-1, "♭"},
        {0, ""},
        {1, "♯"},
    };
};

#endif // TUNING_SCALE_HPP
