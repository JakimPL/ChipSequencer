#ifndef GUI_PANELS_CHANNELS_HPP
#define GUI_PANELS_CHANNELS_HPP

#include <algorithm>
#include <cmath>
#include <string>

#include "../../song/output.hpp"
#include "../init.hpp"
#include "panel.hpp"

class GUIChannelsPanel : public GUIPanel {
  private:
    struct CurrentChannel {
        int envelope_index = 0;
        int order_index = -1;
        int oscillator_index = -1;
        float pitch = 1.0f;
        float transpose = 0.0f;
        OutputType output_type;

        // flag
        bool constant_pitch = false;
        bool sync = false;
        int sync_numerator = 1;
        int sync_denominator = 1;
    } current_channel;

    int channel_index = 0;

    bool is_index_valid() const;
    void draw_channel();
    void update_channel_name(const int index, const int target_id = -1);
    void update_channel_names();

    void from() override;
    void to() const override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIChannelsPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;
};

#endif // GUI_PANELS_CHANNELS_HPP
