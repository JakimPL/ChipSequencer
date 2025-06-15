#pragma once

#include <string>

#include "../constants.hpp"
#include "../init.hpp"
#include "../utils.hpp"
#include "../patterns/pattern.hpp"
#include "../patterns/selection.hpp"
#include "panel.hpp"

class GUISequencesPanel : public GUIPanel {
  private:
    struct CurrentSequence {
        Pattern pattern;
    } current_sequence;

    PatternSelection selection;
    PatternSelectionAction selection_action = PatternSelectionAction::None;
    int transpose_by = 0;

    bool is_index_valid() const;
    void transpose_selected_rows();
    void action();
    void select_all();
    void deselect_all();
    void delete_selection();
    void draw_sequence_length();
    void draw_sequence();

    bool select_item() override;
    void empty() override;
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

    bool is_active() const;

    int sequence_index = 0;
};
