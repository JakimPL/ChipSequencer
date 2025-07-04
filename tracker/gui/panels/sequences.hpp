#pragma once

#include <string>

#include "../../constants.hpp"
#include "../patterns/pattern.hpp"
#include "../patterns/selection.hpp"
#include "panel.hpp"

class GUISequencesPanel : public GUIPanel {
  private:
    struct CurrentSequence {
        Pattern pattern;
    } current_sequence;

    struct EditDialogBox {
        bool visible = false;
        int item = -1;

        int note = NOTE_REST;
    } edit_dialog_box;

    bool dialog_box_open = false;
    PatternSelection selection;
    PatternSelectionAction selection_action = PatternSelectionAction::None;

    bool is_index_valid() const;
    void transpose_selected_rows(int value);
    void select_all();
    void deselect_all();
    void set_selection_note(uint8_t note);
    void delete_selection();
    void copy_selection();
    void paste_selection();

    void perform_notes_action(const std::string &action_name, const PatternSelectionChange<uint8_t> &changes);
    void perform_note_action(int row, uint8_t old_note, uint8_t new_note);

    void draw_sequence_length();
    void draw_sequence();

    void open_edit_dialog_box(int item);

    void register_shortcuts() override;
    bool select_item() override;
    void empty() override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void draw_dialog_box() override;
    void check_keyboard_input() override;
    void shortcut_actions() override;
    void post_actions() override;

  public:
    explicit GUISequencesPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void update() override;

    void set_notes(const std::map<PatternRow, uint8_t> &notes);
    void set_note(const PatternRow &pattern_row, uint8_t note);
    void set_note(int row, uint8_t note);
    void set_index(int index) override;

    void from() override;
    void to() const override;

    bool is_active() const;

    int sequence_index = 0;
};
