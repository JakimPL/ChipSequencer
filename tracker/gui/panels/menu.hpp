#ifndef GUI_PANELS_MENU_HPP
#define GUI_PANELS_MENU_HPP

#include <functional>

class GUIMenu {
  public:
    std::function<void()> callback_new;
    std::function<void()> callback_save;
    std::function<void()> callback_save_as;
    std::function<void()> callback_open;
    std::function<void()> callback_exit;

    GUIMenu();

    void draw();
};

#endif // GUI_PANELS_MENU_HPP