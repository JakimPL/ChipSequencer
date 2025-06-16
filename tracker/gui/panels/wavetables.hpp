#pragma once

#include <algorithm>
#include <cmath>
#include <optional>
#include <string>
#include <vector>

#include "../../audio/wave.hpp"
#include "../init.hpp"
#include "panel.hpp"

class GUIWavetablesPanel : public GUIPanel {
  private:
    struct CurrentWavetable {
        std::vector<float> wave = {};
        int size = 0;
        bool interpolation = false;
    } current_wavetable;

    std::optional<bool> load_status;
    std::optional<bool> render_status;

    float cast_to_float(uint8_t value) const;
    uint8_t cast_to_int(float value) const;

    bool is_index_valid() const;
    void draw_wavetable_length();
    void draw_waveform();
    void draw_status();
    void save_wavetable_to_file();
    void load_wavetable_from_file();
    std::vector<std::vector<float>> prepare_wave_to_save() const;
    void prepare_wave_from_load(Samples samples);

    bool select_item() override;
    void empty() override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;

  public:
    GUIWavetablesPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;

    void update();
    void set_index(const int index) override;

    void from() override;
    void to() const override;

    int wavetable_index = 0;
};
