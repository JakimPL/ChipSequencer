#ifndef GUI_INPUTS_INPUT_HPP
#define GUI_INPUTS_INPUT_HPP

#include <string>
#include <vector>

#include "../constants.hpp"
#include "../init.hpp"

class InputHandler {
  protected:
    std::string buffer;
    int limit = 0;

  public:
    void clear();
    std::string get_buffer() const;
    void set_buffer(const std::string &value);
    void set_limit(const int max);

    bool capture_input = false;
};

#endif // GUI_INPUTS_INPUT_HPP
