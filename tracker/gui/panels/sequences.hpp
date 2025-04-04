#ifndef GUI_PANELS_SEQUENCES_HPP
#define GUI_PANELS_SEQUENCES_HPP

#include <algorithm>
#include <string>

#include "../constants.hpp"
#include "../init.hpp"
#include "../names.hpp"
#include "panel.hpp"

class GUISequencesPanel : public GUIPanel {
  private:
    struct CurrentSequence {
        int steps = 0;
        std::vector<uint8_t> pattern = {};
    } current_sequence;

    int sequence_index = 0;
    int selected_step = 0;
    const int max_steps = GUI_MAX_STEPS;
    int jump_step = 1;

    bool is_index_valid() const;
    std::vector<Note> pattern_to_sequence() const;
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
