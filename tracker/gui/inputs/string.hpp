#ifndef GUI_INPUTS_STRING_HPP
#define GUI_INPUTS_STRING_HPP

#include "input.hpp"

class StringInputHandler : public InputHandler {
  private:
    std::vector<ImGuiKey> keys;
    std::vector<std::string> &strings;

  public:
    StringInputHandler(std::vector<std::string> &strings, int &index, const std::vector<ImGuiKey> keys);
    void handle_input() override;
};

#endif // GUI_INPUTS_STRING_HPP
