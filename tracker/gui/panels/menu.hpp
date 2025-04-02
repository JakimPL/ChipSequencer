#ifndef GUI_PANELS_MENU_HPP
#define GUI_PANELS_MENU_HPP

#include <functional>
#include <optional>

#include "../init.hpp"

class GUIMenu : public GUIPanel {
  private:
    std::optional<bool> compilation_status;

    void file_new();
    void file_save();
    void file_save_as();
    void file_open();
    void file_compile(const bool compress = true);
    void file_exit();

    void from() override {};
    void to() const override {};
    void add() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override {};

  public:
    GUIMenu();
    void update() override {};
};

#endif // GUI_PANELS_MENU_HPP