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

    bool is_index_valid() const;
    void draw_sequence();
    void draw_sequence_length();
    void open_edit_dialog_box(const int item);
    void draw_edit_dialog_box();
    void draw_output_section();
    void set_current_command();

    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUICommandsSequencesPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;

    void from() override;
    void to() const override;

    void clear_input_buffers();

    int sequence_index = 0;
};

#endif // GUI_PANELS_COMMANDS_SEQUENCES_HPP
