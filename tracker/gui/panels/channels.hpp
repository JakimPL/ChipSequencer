#ifndef GUI_PANELS_CHANNELS_HPP
#define GUI_PANELS_CHANNELS_HPP

#include <algorithm>
#include <cmath>
#include <string>
#include "../init.hpp"
#include "../names.hpp"
#include "panel.hpp"
#include "utils.hpp"

class GUIChannelsPanel : GUIPanel {
  private:
    struct CurrentChannel {
        int envelope_index = 0;
        bool constant_pitch = false;
        int order_index = -1;
        int oscillator_index = -1;
        float pitch = 1.0f;
        void *output = nullptr;
        bool additive = false;
        int type = -1;
        int shift = 0;
    } current_channel;

    int channel_index = 0;

    bool is_index_valid() const;
    void draw_channel();

    void from() override;
    void to() const override;
    void add() override;
    void remove() override;

  public:
    GUIChannelsPanel();
    void draw();
    void update();
};

#endif // GUI_PANELS_CHANNELS_HPP
