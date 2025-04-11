#ifndef GUI_PANELS_GENERAL_HPP
#define GUI_PANELS_GENERAL_HPP

#include <atomic>
#include <functional>
#include <string>
#include <thread>

#include "../constants.hpp"
#include "../init.hpp"
#include "panel.hpp"

class GUIGeneralPanel : public GUIPanel {
  private:
    struct CurrentSong {
        char title[GUI_MAX_STRING_LENGTH] = "";
        char author[GUI_MAX_STRING_LENGTH] = "";

        int bpm = DEFAULT_BPM;
        float normalizer = DEFAULT_NORMALIZER;
        int output_channels = DEFAULT_OUTPUT_CHANNELS;

        int edo = DEFAULT_EDO;
        float a4_frequency = DEFAULT_A4_FREQUENCY;
    } current_song;

    bool error = false;
    std::string error_message;

    void draw_play_button();
    void draw_pause_rectangles();
    void draw_play_triangle();
    void draw_stop_square() const;
    void draw_song_info();
    void draw_tuning_settings();
    void play();

    void from() override;
    void to() const override;
    void add() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override {};

  public:
    GUIGeneralPanel();
    void update() override;
};

#endif // GUI_PANELS_GENERAL_HPP
