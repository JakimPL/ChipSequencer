#ifndef GUI_PANELS_COMMANDS_HPP
#define GUI_PANELS_COMMANDS_HPP

#include "../init.hpp"
#include "panel.hpp"

class GUICommandsPanel : public GUIPanel {
  private:
    struct CurrentSequence {
    } current_sequence;

    int commands_index = 0;

    bool is_index_valid() const;
    void draw_sequence();

    void from() override;
    void to() const override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUICommandsPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;
};

#endif // GUI_PANELS_COMMANDS_HPP
