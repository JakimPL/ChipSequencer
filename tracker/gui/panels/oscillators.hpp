#pragma once

#include <string>
#include <vector>

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
    static void update_oscillator_name(int index, int generator_index = -1);
    void update_oscillator_names();

    bool select_item() override;
    void empty() override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;

  public:
    explicit GUIOscillatorsPanel(bool visible = true, bool windowed = true);
    GUIElement get_element() const override;

    void update() override;
    void set_index(int index) override;

    void from() override;
    void to() const override;

    int oscillator_index = 0;
};
