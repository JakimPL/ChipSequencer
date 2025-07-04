#pragma once

#include "panel.hpp"

class GUIDSPsPanel : public GUIPanel {
  private:
    struct CurrentDSP {
        int effect_index = -1;
        std::string type = "";
        OutputType output_type;

        // gainer
        float gainer_gain = 0.5f;

        // distortion
        float distortion_level = 0.5f;

        // filter
        float filter_cutoff = 8000.0f;
        bool filter_mode = false;

        // delay
        float delay_dry = 0.5f;
        float delay_wet = 0.5f;
        float delay_feedback = 0.25f;
        float delay_time = 1.0f;
    } current_dsp;

    bool is_index_valid() const;
    void draw_dsp();
    void draw_dsp_type();
    void draw_effect();
    void update_dsp_names();
    void update_dsp_name(int index, int effect_index = -1) const;

    bool is_disabled() const override;
    bool select_item() override;
    void empty() override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;

  public:
    explicit GUIDSPsPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void update() override;
    void set_index(int index) override;

    void from() override;
    void to() const override;

    int dsp_index = 0;
};
