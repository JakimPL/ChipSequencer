#ifndef GUI_PANELS_COMMANDS_CHANNELS_HPP
#define GUI_PANELS_COMMANDS_CHANNELS_HPP

#include "../panel.hpp"

class GUICommandsChannelsPanel : public GUIPanel {
  private:
    struct CurrentCommandsChannel {
        int order_index = -1;
        bool bypass = false;
        bool hide = false;
    } current_channel;

    bool is_index_valid() const;
    void draw_channel();

    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUICommandsChannelsPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;

    void from() override;
    void to() const override;

    int channel_index = 0;
};

#endif // GUI_PANELS_COMMANDS_CHANNELS_HPP
