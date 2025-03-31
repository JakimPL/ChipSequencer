#ifndef GUI_PANELS_SEQUENCES_HPP
#define GUI_PANELS_SEQUENCES_HPP

#include <algorithm>
#include <string>

#include "../init.hpp"
#include "../names.hpp"
#include "panel.hpp"
#include "utils.hpp"

class GUISequencesPanel : GUIPanel {
  private:
    struct CurrentSequence {
        int steps = 0;
        std::vector<int8_t> pattern = {};
    } current_sequence;

    int sequence_index = 0;
    int selected_step = 0;
    const int max_steps = 64;

    bool is_index_valid() const;
    std::vector<Note> pattern_to_sequence() const;
    void draw_sequence_length();
    void draw_sequence();

    void from() override;
    void to() const override;
    void add() override;
    void remove() override;

  public:
    GUISequencesPanel();
    void draw();
    void update();
};

#endif // GUI_PANELS_SEQUENCES_HPP
