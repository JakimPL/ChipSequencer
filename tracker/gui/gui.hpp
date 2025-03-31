#ifndef GUI_GUI_HPP
#define GUI_GUI_HPP

#include "init.hpp"
#include "panels/channels.hpp"
#include "panels/envelopes.hpp"
#include "panels/general.hpp"
#include "panels/menu.hpp"
#include "panels/oscillators.hpp"
#include "panels/orders.hpp"
#include "panels/sequences.hpp"
#include "panels/wavetables.hpp"
#include <functional>

class GUI {
  public:
    GUI();
    ~GUI();

    bool initialize();
    bool render();
    void set_font();
    void terminate();
    bool is_done();
    void set_play_callback(std::function<void()> callback);
    void set_playing_status(bool status);
    void update();

  private:
    SDL_Window *window;
    SDL_GLContext gl_context;
    ImGuiIO *io;
    ImFont *font;
    bool done = false;
    GUIMenu menu;
    GUIGeneralPanel general_panel;
    GUIChannelsPanel channels_panel;
    GUIEnvelopesPanel envelopes_panel;
    GUIOrdersPanel orders_panel;
    GUIOscillatorsPanel oscillators_panel;
    GUISequencesPanel sequences_panel;
    GUIWavetablesPanel wavetables_panel;
};

#endif // GUI_GUI_HPP
