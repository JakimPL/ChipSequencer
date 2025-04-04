#ifndef GUI_PANELS_GENERAL_HPP
#define GUI_PANELS_GENERAL_HPP

#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include "../init.hpp"
#include "panel.hpp"
#include "utils.hpp"

class GUIGeneralPanel : public GUIPanel {
  private:
    struct CurrentSong {
        char name[64] = "";
        int bpm = DEFAULT_BPM;
        float normalizer = DEFAULT_NORMALIZER;

        int edo = DEFAULT_EDO;
        float a4_frequency = DEFAULT_A4_FREQUENCY;
    } current_song;

    void draw_play_button();
    void draw_pause_rectangles() const;
    void draw_play_triangle() const;
    void draw_stop_square() const;
    void draw_song_info();
    void draw_tuning_settings();

    void from() override;
    void to() const override;
    void add() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override {};

  public:
    GUIGeneralPanel();
    void update() override {};
};

#endif // GUI_PANELS_GENERAL_HPP
