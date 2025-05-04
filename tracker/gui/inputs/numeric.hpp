#ifndef GUI_INPUTS_NUMERIC_HPP
#define GUI_INPUTS_NUMERIC_HPP

#include "input.hpp"

class NumericInputHandler : public InputHandler {
  private:
    std::vector<ImGuiKey> keys;
    std::vector<int> &vector;

  public:
    NumericInputHandler(std::vector<int> &vector, int &index);
    bool handle_input() override;
};

#endif // GUI_INPUTS_NUMERIC_HPP
