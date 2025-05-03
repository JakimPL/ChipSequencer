#ifndef GUI_INPUT_HPP
#define GUI_INPUT_HPP

#include <string>

#include "init.hpp"

class InputHandler {
  private:
    bool numeric = true;
    std::string buffer;
    std::vector<ImGuiKey> keys;
    std::vector<int> &vector;
    int &index;
    int limit = 0;

  public:
    InputHandler(std::vector<int> &vector, int &index);
    void set_limit(const int max);
    void handle_input();
    void clear();
};

#endif // GUI_INPUT_HPP
