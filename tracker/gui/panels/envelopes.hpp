#ifndef GUI_PANELS_ENVELOPES_HPP
#define GUI_PANELS_ENVELOPES_HPP

#include <algorithm>
#include <string>
#include "../init.hpp"
#include "../names.hpp"
#include "utils.hpp"

class GUIEnvelopesPanel {
  private:
    struct CurrentEnvelope {
        float base_volume = 0.0f;
        float sustain_level = 0.0f;
        float attack = 0.0f;
        float decay = 0.0f;
        float hold = 0.0f;
        float release = 0.0f;
    } current_envelope;

    int envelope_index = 0;
    const float max_timer_value = 10.0f;
    const float timer_constant = UINT16_MAX * 0.001f;

    static float cast_to_float(int value, float scale = 1.0f);
    static uint16_t cast_to_int(float value, float scale = 1.0f);

    bool is_index_valid() const;
    void from_envelope();
    void to_envelope() const;
    void draw_levels();
    void draw_timers();
    void draw_envelope_graph();

  public:
    GUIEnvelopesPanel();
    void draw();
    void update();
};

#endif // GUI_PANELS_ENVELOPES_HPP
