#ifndef GUI_PANELS_MENU_HPP
#define GUI_PANELS_MENU_HPP

#include <functional>

#include "../init.hpp"

class GUIMenu {
  public:
    GUIMenu();

    void draw();

  private:
    void file_new();
    void file_save();
    void file_save_as();
    void file_open();
    void file_exit();
};

#endif // GUI_PANELS_MENU_HPP