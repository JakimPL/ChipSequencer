#pragma once

#include <functional>
#include <optional>

#include "panel.hpp"

class GUIEditorPanel : public GUIPanel {
  private:
    std::optional<bool> save_theme_status;
    std::optional<bool> load_theme_error;

    void draw_tabs();
    void draw_options();
    static void draw_history();
    static void draw_clipboard();
    void draw_theme();
    void save_theme();
    void load_theme();

    void draw() override;
    void draw_dialog_box() override;
    void check_keyboard_input() override;

  public:
    explicit GUIEditorPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;
};
