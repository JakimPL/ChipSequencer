#ifndef GUI_PANELS_COMMANDS_CHANNELS_HPP
#define GUI_PANELS_COMMANDS_CHANNELS_HPP

#include "../panel.hpp"

class GUICommandsChannelsPanel : public GUIPanel {
  private:
    struct CurrentCommandsChannel {
        bool bypass = false;
        int order_index = -1;
    } current_channel;

    int channel_index = 0;

    bool is_index_valid() const;
    void draw_channel();

    void from() override;
    void to() const override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUICommandsChannelsPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;
};

#endif // GUI_PANELS_COMMANDS_CHANNELS_HPP
