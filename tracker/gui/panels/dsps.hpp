#ifndef GUI_PANELS_DSPS_HPP
#define GUI_PANELS_DSPS_HPP

#include "../init.hpp"
#include "panel.hpp"

class GUIDSPsPanel : public GUIPanel {
  private:
    struct CurrentDSP {
        int effect_index = -1;
        std::string type = "";

        // gainer
        float gainer_gain = 0.5f;

        // delay
        float delay_dry = 0.5f;
        float delay_wet = 0.5f;
        float delay_feedback = 0.5f;
        float delay_time = 256.0f;

        // filter
        float filter_cutoff = 0.5f;
    } current_dsp;

    int dsp_index = 0;

    bool is_index_valid() const;
    void draw_dsp();
    void draw_dsp_type();

    void from() override;
    void to() const override;
    void add() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIDSPsPanel();
    void update() override;
};

#endif // GUI_PANELS_DSPS_HPP
