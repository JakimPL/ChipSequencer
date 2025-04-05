#ifndef GUI_PANELS_OSCILLATORS_HPP
#define GUI_PANELS_OSCILLATORS_HPP

#include <string>
#include <vector>

#include "../init.hpp"
#include "../names.hpp"
#include "panel.hpp"

class GUIOscillatorsPanel : public GUIPanel {
  private:
    struct CurrentOscillator {
        int generator_index = -1;
        std::string type = "";
        float duty_cycle = 0.0f;
        int wavetable_index = -1;
    } current_oscillator;

    int oscillator_index = 0;
    std::vector<std::string> generator_names;

    bool is_index_valid() const;
    void update_oscillators();
    void update_wavetables();
    void draw_oscillator_type();
    void draw_oscillator();

    void from() override;
    void to() const override;
    void add() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIOscillatorsPanel();
    void update() override;
};

#endif // GUI_PANELS_OSCILLATORS_HPP
