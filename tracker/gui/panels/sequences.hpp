#ifndef GUI_PANELS_SEQUENCES_HPP
#define GUI_PANELS_SEQUENCES_HPP

#include <string>

#include "../constants.hpp"
#include "../init.hpp"
#include "../utils.hpp"
#include "../patterns/pattern.hpp"
#include "panel.hpp"

class GUISequencesPanel : public GUIPanel {
  private:
    struct CurrentSequence {
        Pattern pattern;
    } current_sequence;

    GUIPatternSelection selection;

    bool is_index_valid() const;
    void draw_sequence_length();
    void draw_sequence();

    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUISequencesPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;

    void from() override;
    void to() const override;

    int sequence_index = 0;
};

#endif // GUI_PANELS_SEQUENCES_HPP
