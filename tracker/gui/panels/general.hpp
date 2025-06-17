#pragma once

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
        char message[GUI_MAX_STRING_LENGTH] = DEFAULT_MESSAGE;

        int bpm = DEFAULT_BPM;
        int division = DEFAULT_DIVISION;
        float sample_rate = DEFAULT_SAMPLE_RATE;
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
    void draw_tabs();
    void draw_tempo();
    void draw_output();
    void draw_tuning_settings();
    void play();
    std::string get_error_message(const ValidationResult result, const int index) const;

    void register_shortcuts() override;
    void draw() override;
    void draw_dialog_box() override;

  public:
    GUIGeneralPanel(const bool visible = true, const bool windowed = true);
    GUIElement get_element() const override;

    void update() override;

    void from() override;
    void to() const override;
};
