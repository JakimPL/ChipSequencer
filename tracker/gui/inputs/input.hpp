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
    virtual void clear() = 0;
    virtual bool handle_input() = 0;
    void set_limit(const int max);

    std::string get_buffer() const;
    void set_buffer(const std::string &value);
};

#endif // GUI_INPUTS_INPUT_HPP
