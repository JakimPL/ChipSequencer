#ifndef GUI_GUI_HPP
#define GUI_GUI_HPP

#include <functional>

#include "../audio/engine.hpp"
#include "enums.hpp"
#include "init.hpp"
#include "panels/channels.hpp"
#include "panels/dsps.hpp"
#include "panels/editor.hpp"
#include "panels/envelopes.hpp"
#include "panels/general.hpp"
#include "panels/menu.hpp"
#include "panels/oscillators.hpp"
#include "panels/orders.hpp"
#include "panels/patterns.hpp"
#include "panels/routing.hpp"
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

    void frame();
    void play() const;
    void stop() const;
    bool is_playing() const;
    bool is_paused() const;

    void save(const std::string &filename);
    void open(const std::string &filename);

    void set_index(const GUIElement element, const int index);
    void change_window_title(const std::string &title);
    void set_visibility(const GUIElement element, const bool visible);
    bool get_visibility(const GUIElement element) const;

    void set_audio_engine(AudioEngine *engine);
    void update(GUIElement element = GUIElement::All);

    int get_current_octave() const;
    int get_jump_step() const;
    int get_page_size() const;

    void deselect_all_rows();

    std::array<std::array<Note, GUI_MAX_STEPS>, MAX_SEQUENCES> sequences_buffer;
    std::array<std::array<uint8_t, GUI_MAX_WAVETABLE_POINTS>, MAX_WAVETABLES> wavetables_buffer;

  private:
    AudioEngine *audio_engine;

    GUIMenu menu;
    GUIEditorPanel editor = GUIEditorPanel(true, current_octave, jump_step, page_size);
    GUIGeneralPanel general_panel;
    GUIChannelsPanel channels_panel;
    GUIDSPsPanel dsps_panel;
    GUIEnvelopesPanel envelopes_panel;
    GUIOrdersPanel orders_panel;
    GUIOscillatorsPanel oscillators_panel;
    GUISequencesPanel sequences_panel;
    GUIWavetablesPanel wavetables_panel;
    GUIPatternsPanel patterns_panel;
    GUIRoutingPanel routing_panel;

    SDL_Window *window;
    SDL_GLContext gl_context;
    ImGuiIO *io;
    ImFont *font;
    bool done = false;

    int current_octave = GUI_DEFAULT_CURRENT_OCTAVE;
    int jump_step = GUI_DEFAULT_JUMP_STEP;
    int page_size = GUI_DEFAULT_PAGE_SIZE;

    void update_all();
    void frame_all();
    void set_visibility_all(const bool visible);
};

#endif // GUI_GUI_HPP
