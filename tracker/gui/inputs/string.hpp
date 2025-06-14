#pragma once

#include "input.hpp"

class StringInputHandler : public InputHandler {
  private:
    bool synchronize;
    std::vector<ImGuiKey> keys;

  public:
    StringInputHandler(const std::vector<ImGuiKey> keys, const bool synchronize = true);
    bool handle_input(std::vector<std::string> &strings, int &index);
};
