#ifndef GUI_PANELS_MENU_HPP
#define GUI_PANELS_MENU_HPP

#include <functional>
#include <optional>

#include "../../song/compilation.hpp"
#include "../init.hpp"
#include "../shortcuts.hpp"

class GUIMenu : public GUIPanel {
  private:
    std::optional<bool> compilation_status;
    std::optional<bool> render_status;
    std::optional<bool> load_error;

    void file_new_confirm();
    void file_new();
    void file_save();
    void file_save_as();
    void file_open();
    void file_render();
    void file_compile(const CompilationScheme scheme, const CompilationTarget compilation_target);
    void file_exit();

    void from() override {};
    void to() const override {};
    void add() override {};
    void duplicate() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override {};

  public:
    GUIMenu(const bool visible = true);
    void update() override {};
    void set_index(const int index) override {};
};

#endif // GUI_PANELS_MENU_HPP