#pragma once

#include "input.hpp"

class StringInputHandler : public InputHandler {
  private:
    bool synchronize;
    std::vector<ImGuiKey> keys;

  public:
    explicit StringInputHandler(std::vector<ImGuiKey> keys, bool synchronize = true);
    bool handle_input(std::vector<std::string> &strings, int &index);
};
