#pragma once

#include <functional>
#include <string>

#include "../audio/engine.hpp"
#include "../song/validation.hpp"
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
#include "panels/routings.hpp"
#include "panels/sequences.hpp"
#include "panels/summary.hpp"
#include "panels/waveform.hpp"
#include "panels/wavetables.hpp"
#include "panels/commands/channels.hpp"
#include "panels/commands/sequences.hpp"

class GUI {
  public:
    GUI();
    ~GUI();

    bool initialize();
    bool render();
    void set_font();
    void terminate();
    bool is_done();
    void from();
    void to() const;

    void frame();
    std::pair<ValidationResult, int> pre_play() const;
    std::pair<ValidationResult, int> play() const;
    std::pair<ValidationResult, int> play_from(const uint16_t row, const bool restart) const;
    void stop(const bool restore_parameters = true) const;
    bool is_playing() const;
    bool is_paused() const;

    const AudioHistory &get_audio_history() const;
    void lock_audio_history() const;
    void unlock_audio_history() const;

    void new_song();
    void save(const std::string &filename);
    void open(const std::string &filename);

    void set_index(const GUIElement element, const int index);
    void change_window_title(const std::string &title = "");
    void set_visibility(const GUIElement element, const bool visible);
    bool get_visibility(const GUIElement element) const;

    void set_audio_engine(AudioEngine *engine);
    bool check_audio_error() const;

    void update(GUIElement element = GUIElement::All);

    int get_current_octave() const;
    int get_jump_step() const;
    int get_page_size() const;
    int get_current_page() const;
    int get_current_row() const;
    std::pair<int, int> get_page_start_end(const int page) const;

    int get_current_channel_index() const;
    int get_current_dsp_index() const;
    int get_current_commands_channel_index() const;
    int get_current_oscillator_index() const;
    int get_current_envelope_index() const;
    int get_current_sequence_index() const;
    int get_current_order_index() const;
    int get_current_wavetable_index() const;
    int get_current_commands_sequence_index() const;

    void set_current_octave(const int octave = GUI_DEFAULT_CURRENT_OCTAVE);
    void set_jump_step(const int step = GUI_DEFAULT_JUMP_STEP);
    void set_page_size(const int size = GUI_DEFAULT_PAGE_SIZE);

    void set_current_channel_index(const int index);
    void set_current_dsp_index(const int index);
    void set_current_commands_channel_index(const int index);
    void set_current_oscillator_index(const int index);
    void set_current_envelope_index(const int index);
    void set_current_sequence_index(const int index);
    void set_current_order_index(const int index);
    void set_current_wavetable_index(const int index);
    void set_current_commands_sequence_index(const int index);

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

    GUIMenu menu = GUIMenu(true, false);
    GUIEditorPanel editor;
    GUIGeneralPanel general_panel;
    GUIChannelsPanel channels_panel;
    GUICommandsChannelsPanel commands_channels_panel;
    GUICommandsSequencesPanel commands_sequences_panel;
    GUIDSPsPanel dsps_panel;
    GUIEnvelopesPanel envelopes_panel;
    GUIOrdersPanel orders_panel;
    GUIOscillatorsPanel oscillators_panel;
    GUISequencesPanel sequences_panel;
    GUIWavetablesPanel wavetables_panel;
    GUIPatternsPanel patterns_panel;
    GUIRoutingsPanel routing_panel;
    GUISummaryPanel summary_panel;
    GUIWaveformPanel waveform_panel;

    SDL_Window *window;
    SDL_GLContext gl_context;
    ImGuiIO *io;
    ImFont *font;
    bool done = false;

    void update_all();
    void frame_all();
    void set_visibility_all(const bool visible);
    void clear_input_buffers();
};
