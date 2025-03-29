#ifndef GUI_PANELS_SEQUENCES_HPP
#define GUI_PANELS_SEQUENCES_HPP

#include <algorithm>
#include <string>
#include "../init.hpp"
#include "../names.hpp"
#include "utils.hpp"

class GUISequencesPanel {
  private:
    struct CurrentSequence {
        int steps = 0;
        std::vector<int8_t> pattern = {};
    } current_sequence;

    int sequence_index = 0;
    int selected_step = 0;
    const int max_steps = 64;

    void from_sequence();
    std::vector<Note> pattern_to_sequence();
    void to_sequence();
    void update_sequences();
    void draw_sequence_length();
    void draw_sequence();

  public:
    GUISequencesPanel();
    void draw();
};

#endif // GUI_PANELS_SEQUENCES_HPP
