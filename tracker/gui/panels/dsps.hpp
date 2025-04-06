#ifndef GUI_PANELS_DSPS_HPP
#define GUI_PANELS_DSPS_HPP

#include "../init.hpp"
#include "panel.hpp"

class GUIDSPsPanel : public GUIPanel {
  private:
    struct CurrentDSP {
        // gainer

    } current_dsp;

    int dsp_index = 0;

    bool is_index_valid() const;
    void draw_dsp();

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
