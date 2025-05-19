#ifndef GUI_INPUTS_STRING_HPP
#define GUI_INPUTS_STRING_HPP

#include "input.hpp"

class StringInputHandler : public InputHandler {
  private:
    bool synchronize;
    std::vector<ImGuiKey> keys;
    std::vector<std::string> &strings;

  public:
    StringInputHandler(std::vector<std::string> &strings, int &index, const std::vector<ImGuiKey> keys, const bool synchronize = true);
    void clear() override;
    bool handle_input() override;

    bool capture_input = false;
};

#endif // GUI_INPUTS_STRING_HPP
