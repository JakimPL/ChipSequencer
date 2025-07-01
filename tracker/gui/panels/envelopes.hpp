#pragma once

#include <algorithm>
#include <string>

#include "../constants.hpp"
#include "panel.hpp"

class GUIEnvelopesPanel : public GUIPanel {
  private:
    struct CurrentEnvelope {
        float base_volume = 0.0f;
        float sustain_level = 0.0f;
        float attack = 0.0f;
        float decay = 0.0f;
        float hold = 0.0f;
        float release = 0.0f;

        std::vector<float> timers;
        std::vector<float> positions;
    } current_envelope;

    const float timer_constant = static_cast<float>(MAX_ENVELOPE_TIMER_LENGTH);

    static float cast_to_float(int value, float scale = 1.0f);
    static uint16_t cast_to_int(float value, float scale = 1.0f);

    bool is_index_valid() const;
    void draw_envelope();
    void draw_levels();
    void draw_timers();
    void draw_envelope_graph();
    void gather_envelope_positions();

    bool select_item() override;
    void empty() override;
    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;

  public:
    GUIEnvelopesPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;

    void update() override;
    void set_index(const int index) override;

    void from() override;
    void to() const override;

    int envelope_index = 0;
};
