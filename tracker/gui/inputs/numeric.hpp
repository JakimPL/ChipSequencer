#pragma once

#include "input.hpp"

class NumericInputHandler : public InputHandler {
  private:
    std::vector<ImGuiKey> keys;
    void assign_value(std::vector<int> &vector, size_t index, int adjustment = 0);

  public:
    bool handle_input(std::vector<int> &vector, int &index);
};
