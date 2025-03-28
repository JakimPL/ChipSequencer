#ifndef GUI_PANELS_GENERAL_HPP
#define GUI_PANELS_GENERAL_HPP

#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include "../init.hpp"
#include "utils.hpp"

class GUIGeneralPanel {
  private:
    struct CurrentSong {
        char name[64] = "";
        int bpm = 120;
        float normalizer = 0.5f;
    } current_song;

    void from_song();
    void to_song();
    void draw_play_button();
    void draw_song_info();

  public:
    std::function<void()> play_callback;
    std::atomic<bool> is_playing;

    GUIGeneralPanel();
    void draw();
};

#endif // GUI_PANELS_GENERAL_HPP
