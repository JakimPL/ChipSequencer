#ifndef GUI_PANELS_WAVETABLES_HPP
#define GUI_PANELS_WAVETABLES_HPP

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include "../init.hpp"
#include "../names.hpp"
#include "utils.hpp"

class GUIWavetablesPanel {
  private:
    struct CurrentWavetable {
        std::vector<float> wave = {};
        int size = 0;
        bool interpolate = false;
    } current_wavetable;

    int wavetable_index = 0;
    const int max_points = 128;

    bool is_index_valid() const;
    void from_wavetable();
    void to_wavetable();
    void draw_wavetable_length();
    void draw_waveform();

  public:
    GUIWavetablesPanel();
    void draw();
    void update();
};

#endif // GUI_PANELS_WAVETABLES_HPP
