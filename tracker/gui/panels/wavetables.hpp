#ifndef GUI_PANELS_WAVETABLES_HPP
#define GUI_PANELS_WAVETABLES_HPP

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "../init.hpp"
#include "panel.hpp"

class GUIWavetablesPanel : public GUIPanel {
  private:
    struct CurrentWavetable {
        std::vector<float> wave = {};
        int size = 0;
        bool interpolation = false;
    } current_wavetable;

    int wavetable_index = 0;

    float cast_to_float(uint8_t value) const;
    uint8_t cast_to_int(float value) const;

    bool is_index_valid() const;
    void draw_wavetable_length();
    void draw_waveform();
    void save_wavetable_to_file() const;
    void load_wavetable_from_file();

    void from() override;
    void to() const override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIWavetablesPanel(const bool visible = true);
    void update();
    void set_index(const int index) override;
};

#endif // GUI_PANELS_WAVETABLES_HPP
