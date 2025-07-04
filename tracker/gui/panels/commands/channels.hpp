#pragma once

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

    bool is_disabled() const override;
    bool select_item() override;
    void empty() override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;

  public:
    explicit GUICommandsChannelsPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void update() override;
    void set_index(int index) override;

    void from() override;
    void to() const override;

    int channel_index = 0;
};
