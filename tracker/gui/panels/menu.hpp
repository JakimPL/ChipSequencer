#pragma once

#include <functional>
#include <optional>

#include "../../song/compilation.hpp"
#include "../shortcuts/manager.hpp"
#include "panel.hpp"

class GUIMenu : public GUIPanel {
  private:
    std::string compilation_error;
    std::optional<bool> compilation_status;
    std::optional<bool> render_status;
    std::optional<bool> load_error;

    bool open_new_song_confirmation_popup = false;
    bool open_open_song_confirmation_popup = false;
    bool open_exit_confirmation_popup = false;

    bool commands_view_active = false;
    GUIElement active_window = GUIElement::Count;

    void draw_menu();
    static void file_new();
    static void file_save();
    static void file_save_as();
    void file_open();
    void file_render();
    void file_compile(CompilationScheme scheme, CompilationTarget compilation_target);
    static void file_exit();

    void file_new_confirm();
    void file_open_confirm();
    void file_exit_confirm();

    void register_shortcuts() override;
    void draw() override;
    void draw_dialog_box() override;

  public:
    explicit GUIMenu(bool visible = true, bool windowed = false);
    GUIElement get_element() const override;
};
