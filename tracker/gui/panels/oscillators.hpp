#ifndef GUI_PANELS_OSCILLATORS_HPP
#define GUI_PANELS_OSCILLATORS_HPP

#include <algorithm>
#include <string>
#include "../init.hpp"
#include "../names.hpp"
#include "utils.hpp"

class GUIOscillatorsPanel {
  private:
    struct CurrentOscillator {
        int generator_index;
        std::string type;
        float duty_cycle;
        int wavetable_index;
    } current_oscillator;

    int oscillator_index = 0;
    std::vector<std::string> generator_names;

    void from_oscillator();
    void to_oscillator();
    void update_oscillators();
    void update_wavetables();
    void draw_oscillator_type();
    void draw_oscillator();

  public:
    GUIOscillatorsPanel();
    void draw();
};

#endif // GUI_PANELS_OSCILLATORS_HPP
