#pragma once

#include "../../constants.hpp"
#include "../../utils.hpp"
#include "../../patterns/commands.hpp"
#include "../../patterns/selection.hpp"
#include "../panel.hpp"

class GUICommandsSequencesPanel : public GUIPanel {
  private:
    struct CurrentCommandsSequence {
        CommandsPattern pattern;
    } current_sequence;

    struct EditDialogBox {
        bool visible = false;
        int item = -1;

        int instruction = 0;
        int portamento_channel = -1;
        float portamento_value = 0.0f;
        float gainer = 0.5f;
        int bpm = DEFAULT_BPM;
        int division = DEFAULT_DIVISION;

        OutputType output_type;
        int value_integer = 0;
        float value_float = 0.0f;
    } edit_dialog_box;

    bool dialog_box_open = false;
    PatternSelection selection;
    PatternSelectionAction selection_action = PatternSelectionAction::None;

    bool is_index_valid() const;
    void select_all();
    void deselect_all();
    void delete_selection();
    void copy_selection();
    void paste_selection();
    void set_current_command();

    void perform_commands_action(const std::string &action_name, const PatternSelectionChange<CommandValue> &changes);
    void perform_command_action(int row, const CommandValue &old_command, const CommandValue &new_command);

    void draw_sequence();
    void draw_sequence_length();

    void open_edit_dialog_box(int item);
    void draw_output_section();

    void register_shortcuts() override;
    bool is_disabled() const override;
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
    GUICommandsSequencesPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void update() override;

    void set_commands(const std::map<PatternRow, CommandValue> &commands_values);
    void set_command(const PatternRow &pattern_row, const CommandValue &command_value);
    void set_command(const PatternRow &pattern_row, const std::string &command, const std::string &value);
    void set_command(int row, const CommandValue &command_value);
    void set_command(int row, const std::string &command, const std::string &value);
    void set_index(int index) override;

    void from() override;
    void to() const override;

    void clear_input_buffers();
    bool is_active() const;

    int sequence_index = 0;
};
