#ifndef GUI_INPUTS_NUMERIC_HPP
#define GUI_INPUTS_NUMERIC_HPP

#include "input.hpp"

class NumericInputHandler : public InputHandler {
  private:
    std::vector<ImGuiKey> keys;

  public:
    bool handle_input(std::vector<int> &vector, int &index);
};

#endif // GUI_INPUTS_NUMERIC_HPP
