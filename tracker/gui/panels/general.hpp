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
        int bpm = 120;
        float normalizer = 0.5f;
    } current_song;

    void draw_play_button();
    void draw_song_info();

    void from() override;
    void to() const override;
    void add() override {};
    void remove() override {};
    void draw() override;
    void check_keyboard_input() override {};

  public:
    std::function<void()> play_callback;
    std::atomic<bool> is_playing;

    GUIGeneralPanel();
    void update() override {};
};

#endif // GUI_PANELS_GENERAL_HPP
