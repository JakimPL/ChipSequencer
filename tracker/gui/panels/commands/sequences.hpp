#ifndef GUI_PANELS_COMMANDS_SEQUENCES_HPP
#define GUI_PANELS_COMMANDS_SEQUENCES_HPP

#include "../../constants.hpp"
#include "../../init.hpp"
#include "../../patterns/commands.hpp"
#include "../panel.hpp"

class GUICommandsSequencesPanel : public GUIPanel {
  private:
    struct CurrentCommandsSequence {
        CommandsPattern pattern;
    } current_sequence;

    int sequence_index = 0;

    bool is_index_valid() const;
    void draw_sequence();
    void draw_sequence_length();

    void from() override;
    void to() const override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUICommandsSequencesPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;
};

#endif // GUI_PANELS_COMMANDS_SEQUENCES_HPP
