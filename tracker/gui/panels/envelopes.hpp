#ifndef GUI_PANELS_ENVELOPES_HPP
#define GUI_PANELS_ENVELOPES_HPP

#include <algorithm>
#include <string>

#include "../constants.hpp"
#include "../init.hpp"
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
    void draw_levels();
    void draw_timers();
    void draw_envelope_graph();
    void gather_envelope_positions();

    void add() override;
    void duplicate() override;
    void remove() override;
    void draw() override;
    void check_keyboard_input() override;

  public:
    GUIEnvelopesPanel(const bool visible = true);
    void update() override;
    void set_index(const int index) override;

    void from() override;
    void to() const override;

    int envelope_index = 0;
};

#endif // GUI_PANELS_ENVELOPES_HPP
