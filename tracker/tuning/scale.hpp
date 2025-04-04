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
    std::string render(const std::string &name, int offset) const;
    void get_note_centers_and_limits();

    int edo = 0;
    int a_index = -1;
    std::vector<double> centers;
    std::vector<double> limits;
    std::vector<std::string> scale;
    const std::map<int, std::string> symbols;
};

#endif // TUNING_SCALE_HPP
