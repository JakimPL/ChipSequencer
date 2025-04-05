#ifndef GUI_PANELS_SEQUENCES_HPP
#define GUI_PANELS_SEQUENCES_HPP

#include <string>

#include "../constants.hpp"
#include "../init.hpp"
#include "../names.hpp"
#include "../pattern.hpp"
#include "../utils.hpp"
#include "panel.hpp"

class GUISequencesPanel : public GUIPanel {
  private:
    struct CurrentSequence {
        Pattern pattern;
    } current_sequence;

    int sequence_index = 0;
    const int max_steps = GUI_MAX_STEPS;
    int jump_step = 1;

    bool is_index_valid() const;
    void draw_sequence_length();
    void draw_sequence();

    void jump();
    void set_note(const int note_index, const int edo);

    void from() override;
    void to() const override;
    void add() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUISequencesPanel();
    void update() override;
};

#endif // GUI_PANELS_SEQUENCES_HPP
