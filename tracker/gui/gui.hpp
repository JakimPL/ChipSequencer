#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include "../audio/engine.hpp"
#include "../song/validation.hpp"
#include "constants.hpp"
#include "enums.hpp"
#include "node.hpp"
#include "patterns/display.hpp"

class ImGuiIO;
struct ImFont;
class GUIMenu;
class GUIEditorPanel;
class GUIGeneralPanel;
class GUIChannelsPanel;
class GUICommandsChannelsPanel;
class GUICommandsSequencesPanel;
class GUIDSPsPanel;
class GUIEnvelopesPanel;
class GUIOrdersPanel;
class GUIOscillatorsPanel;
class GUISequencesPanel;
class GUIWavetablesPanel;
class GUIPatternsPanel;
class GUIRoutingsPanel;
class GUISummaryPanel;
class GUIWaveformPanel;

class GUI {
  public:
    GUI();
    ~GUI();

    bool initialize();
    bool render();
    void set_font();
    void terminate();
    bool is_done();
    void toggle_fullscreen();
    void from();
    void to() const;

    void frame();
    std::pair<ValidationResult, int> pre_play() const;
    std::pair<ValidationResult, int> play() const;
    std::pair<ValidationResult, int> play_from(uint16_t row, bool restart) const;
    void stop(bool restore_parameters = true) const;
    bool is_playing() const;
    bool is_paused() const;
    bool is_fullscreen() const;

    const AudioHistory &get_audio_history() const;
    void lock_audio_history() const;
    void unlock_audio_history() const;

    void new_song();
    void save(const std::string &filename);
    void open(const std::string &filename);

    void set_index(GUIElement element, int index);
    void change_window_title(const std::string &title = "");
    void set_visibility(GUIElement element, bool visible);
    bool get_visibility(GUIElement element) const;

    void set_audio_engine(AudioEngine *engine);
    bool check_audio_error() const;

    void update(GUIElement element = GUIElement::All);

    int get_current_octave() const;
    int get_jump_step() const;
    int get_page_size() const;
    int get_current_page() const;
    int get_current_row() const;
    std::pair<int, int> get_page_start_end(int page) const;

    int get_current_channel_index() const;
    int get_current_dsp_index() const;
    int get_current_commands_channel_index() const;
    int get_current_oscillator_index() const;
    int get_current_envelope_index() const;
    int get_current_sequence_index() const;
    int get_current_order_index() const;
    int get_current_wavetable_index() const;
    int get_current_commands_sequence_index() const;

    void set_current_octave(int octave = GUI_DEFAULT_CURRENT_OCTAVE);
    void set_jump_step(int step = GUI_DEFAULT_JUMP_STEP);
    void set_page_size(int size = GUI_DEFAULT_PAGE_SIZE);

    void set_current_channel_index(int index);
    void set_current_dsp_index(int index);
    void set_current_commands_channel_index(int index);
    void set_current_oscillator_index(int index);
    void set_current_envelope_index(int index);
    void set_current_sequence_index(int index);
    void set_current_order_index(int index);
    void set_current_wavetable_index(int index);
    void set_current_commands_sequence_index(int index);

    void clear_routing_nodes();
    std::vector<std::pair<NodeIdentifier, ImVec2>> get_routing_nodes_positions() const;
    void set_routing_nodes_positions(const std::vector<std::pair<NodeIdentifier, ImVec2>> &nodes_positions);

    void deselect_all_rows();

    bool is_pattern_view_active() const;
    bool is_commands_pattern_view_active() const;

    bool follow_playback = true;
    int current_octave = GUI_DEFAULT_CURRENT_OCTAVE;
    int jump_step = GUI_DEFAULT_JUMP_STEP;
    int page_size = GUI_DEFAULT_PAGE_SIZE;

    bool repeat_patterns = false;
    int row_display_index = 0;
    RowDisplayStyle row_display = RowDisplayStyle::Page;

  private:
    AudioEngine *audio_engine;
    AudioHistory empty_history;

    std::unique_ptr<GUIMenu> menu;
    std::unique_ptr<GUIEditorPanel> editor;
    std::unique_ptr<GUIGeneralPanel> general_panel;
    std::unique_ptr<GUIChannelsPanel> channels_panel;
    std::unique_ptr<GUICommandsChannelsPanel> commands_channels_panel;
    std::unique_ptr<GUICommandsSequencesPanel> commands_sequences_panel;
    std::unique_ptr<GUIDSPsPanel> dsps_panel;
    std::unique_ptr<GUIEnvelopesPanel> envelopes_panel;
    std::unique_ptr<GUIOrdersPanel> orders_panel;
    std::unique_ptr<GUIOscillatorsPanel> oscillators_panel;
    std::unique_ptr<GUISequencesPanel> sequences_panel;
    std::unique_ptr<GUIWavetablesPanel> wavetables_panel;
    std::unique_ptr<GUIPatternsPanel> patterns_panel;
    std::unique_ptr<GUIRoutingsPanel> routing_panel;
    std::unique_ptr<GUISummaryPanel> summary_panel;
    std::unique_ptr<GUIWaveformPanel> waveform_panel;

    RenderingBackend rendering_backend;
    SDL_Window *window;
    SDL_GLContext gl_context;
    SDL_Renderer *renderer;
    ImGuiIO *io;
    ImFont *font;
    bool done = false;
    bool fullscreen = false;

    bool try_opengl_es();
    bool try_opengl_core();
    bool initialize_imgui_opengl(const char *glsl_version);
    bool initialize_imgui_software();
    bool initialize_imgui_common();
    bool render_opengl();

#if HAVE_SDL_RENDERER
    bool try_software_renderer();
    bool render_software();
#endif

    void initialize_all();
    void update_all();
    void frame_all();
    void set_visibility_all(bool visible);
    void clear_input_buffers();
};
