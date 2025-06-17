#pragma once

#include <functional>
#include <optional>

#include "../../song/compilation.hpp"
#include "../init.hpp"
#include "../history/shortcuts.hpp"
#include "panel.hpp"

class GUIMenu : public GUIPanel {
  private:
    std::optional<bool> compilation_status;
    std::optional<bool> render_status;
    std::optional<bool> load_error;

    bool open_new_song_confirmation_popup = false;
    bool open_open_song_confirmation_popup = false;
    bool open_exit_confirmation_popup = false;

    void draw_menu();
    void file_new();
    void file_save();
    void file_save_as();
    void file_open();
    void file_render();
    void file_compile(const CompilationScheme scheme, const CompilationTarget compilation_target);
    void file_exit();

    void file_new_confirm();
    void file_open_confirm();
    void file_exit_confirm();

    void register_shortcuts() override;
    void draw() override;
    void draw_dialog_box() override;

  public:
    GUIMenu(const bool visible = true, const bool windowed = false);
    GUIElement get_element() const override;
};
