#ifndef GUI_INPUT_HPP
#define GUI_INPUT_HPP

#include <string>
#include <vector>

#include "init.hpp"

class InputHandler {
  private:
    bool numeric = true;
    std::string buffer;
    std::vector<ImGuiKey> keys;
    std::vector<int> &vector;
    std::vector<std::string> &strings;
    int &index;
    int limit = 0;

    std::vector<int> dvector;
    std::vector<std::string> svector;

  public:
    InputHandler(std::vector<int> &vector, int &index);
    InputHandler(std::vector<std::string> &strings, int &index, const std::vector<ImGuiKey> keys);
    void set_limit(const int max);
    void handle_input();
    void clear();
};

#endif // GUI_INPUT_HPP
