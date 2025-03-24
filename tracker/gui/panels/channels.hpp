#ifndef GUI_PANELS_CHANNELS_HPP
#define GUI_PANELS_CHANNELS_HPP

#include <algorithm>
#include <cmath>
#include <string>
#include "../init.hpp"
#include "../names.hpp"
#include "utils.hpp"

class GUIChannelsPanel {
  private:
    struct CurrentChannel {
        int envelope_index;
        bool constant_pitch;
        int order_index;
        int oscillator_index;
        float pitch;
        void *output;
        bool additive;
        int type;
        int shift;
    } current_channel;

    int channel_index = 0;

    void from_channel();
    void to_channel();
    void update_channels();
    void draw_channel();

  public:
    GUIChannelsPanel();
    void draw();
};

#endif // GUI_PANELS_CHANNELS_HPP
