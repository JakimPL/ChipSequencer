#pragma once

#include <string>
#include <vector>

#include "../constants.hpp"
#include "../init.hpp"

class InputHandler {
  protected:
    std::string buffer;
    int limit = 0;

  public:
    void clear(bool capture = true);
    std::string get_buffer() const;
    void set_buffer(const std::string &value);
    void set_limit(int max);

    bool capture_input = false;
};
