#pragma once

#include <array>
#include <string>
#include <vector>
#include <map>

#include "constants.hpp"

class ScaleComposer {
  public:
    void compose(const int new_edo);

    int get_edo() const;
    size_t get_a_index() const;
    std::array<std::string, MAX_EDO> get_scale() const;

  private:
    int edo = 0;
    int a_index = -1;
    std::array<std::string, MAX_EDO> scale;

    ptrdiff_t get_ordered_index(const std::string &note_name);
    std::string render(const std::string &name, int offset) const;
};
