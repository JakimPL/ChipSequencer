#pragma once

#include <algorithm>
#include <cmath>
#include <string>

#include "../../song/output.hpp"
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
        bool hide = false;
        int sync_numerator = 1;
        int sync_denominator = 1;
    } current_channel;

    bool is_index_valid() const;
    void draw_channel();
    void update_channel_name(int index, int target_id = -1);
    void update_channel_names();

    bool is_disabled() const override;
    bool select_item() override;
    void empty() override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;

  public:
    GUIChannelsPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void update() override;
    void set_index(int index) override;

    void from() override;
    void to() const override;

    int channel_index = 0;
};
