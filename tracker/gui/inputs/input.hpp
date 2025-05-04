#ifndef GUI_INPUTS_INPUT_HPP
#define GUI_INPUTS_INPUT_HPP

#include <string>
#include <vector>

#include "../constants.hpp"
#include "../init.hpp"

class InputHandler {
  protected:
    std::string buffer;
    int &index;
    int limit = 0;

  public:
    InputHandler(int &index);
    virtual void handle_input() = 0;
    void set_limit(const int max);
    void clear();
};

#endif // GUI_INPUTS_INPUT_HPP
