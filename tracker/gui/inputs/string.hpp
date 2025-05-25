#ifndef GUI_INPUTS_STRING_HPP
#define GUI_INPUTS_STRING_HPP

#include "input.hpp"

class StringInputHandler : public InputHandler {
  private:
    bool synchronize;
    std::vector<ImGuiKey> keys;

  public:
    StringInputHandler(const std::vector<ImGuiKey> keys, const bool synchronize = true);
    bool handle_input(std::vector<std::string> &strings, int &index);
};

#endif // GUI_INPUTS_STRING_HPP
