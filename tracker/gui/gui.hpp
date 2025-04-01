#ifndef GUI_GUI_HPP
#define GUI_GUI_HPP

#include <functional>

#include "init.hpp"
#include "panels/channels.hpp"
#include "panels/editor.hpp"
#include "panels/envelopes.hpp"
#include "panels/general.hpp"
#include "panels/menu.hpp"
#include "panels/oscillators.hpp"
#include "panels/orders.hpp"
#include "panels/sequences.hpp"
#include "panels/wavetables.hpp"

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

    int current_octave = 3;
    int get_current_octave() const;

    GUIMenu menu;
    GUIEditor editor = GUIEditor(current_octave);
    GUIGeneralPanel general_panel;
    GUIChannelsPanel channels_panel;
    GUIEnvelopesPanel envelopes_panel;
    GUIOrdersPanel orders_panel;
    GUIOscillatorsPanel oscillators_panel;
    GUISequencesPanel sequences_panel;
    GUIWavetablesPanel wavetables_panel;

  private:
    SDL_Window *window;
    SDL_GLContext gl_context;
    ImGuiIO *io;
    ImFont *font;
    bool done = false;
};

#endif // GUI_GUI_HPP
