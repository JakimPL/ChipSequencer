#ifndef GUI_PANELS_OSCILLATORS_HPP
#define GUI_PANELS_OSCILLATORS_HPP

#include <string>
#include <vector>

#include "../init.hpp"
#include "panel.hpp"

class GUIOscillatorsPanel : public GUIPanel {
  private:
    struct CurrentOscillator {
        int generator_index = -1;
        std::string type = "";

        // square
        float square_duty_cycle = 0.5f;

        // saw
        bool saw_reverse = false;

        // wavetable
        int wavetable_index = -1;
        bool wavetable_interpolation = false;
    } current_oscillator;

    bool is_index_valid() const;
    void draw_oscillator_type();
    void draw_oscillator();
    void update_wavetables();
    void update_oscillator_name(const int index, const int generator_index = -1);
    void update_oscillator_names();

    void from() override;
    void to() const override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIOscillatorsPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;

    int oscillator_index = 0;
};

#endif // GUI_PANELS_OSCILLATORS_HPP
