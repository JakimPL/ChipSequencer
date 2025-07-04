#include <algorithm>
#include <iostream>

#include "../imgui/imgui_internal.h"

#include "../general.hpp"
#include "../song/song.hpp"
#include "../song/links/manager.hpp"
#include "../tuning/frequencies.hpp"
#include "../utils/math.hpp"
#include "default.hpp"
#include "gui.hpp"
#include "init.hpp"
#include "utils.hpp"
#include "history/manager.hpp"
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

GUI::GUI() {
    menu = std::make_unique<GUIMenu>(true, false);
    editor = std::make_unique<GUIEditorPanel>();
    general_panel = std::make_unique<GUIGeneralPanel>();
    channels_panel = std::make_unique<GUIChannelsPanel>();
    commands_channels_panel = std::make_unique<GUICommandsChannelsPanel>();
    commands_sequences_panel = std::make_unique<GUICommandsSequencesPanel>();
    dsps_panel = std::make_unique<GUIDSPsPanel>();
    envelopes_panel = std::make_unique<GUIEnvelopesPanel>();
    orders_panel = std::make_unique<GUIOrdersPanel>();
    oscillators_panel = std::make_unique<GUIOscillatorsPanel>();
    sequences_panel = std::make_unique<GUISequencesPanel>();
    wavetables_panel = std::make_unique<GUIWavetablesPanel>();
    patterns_panel = std::make_unique<GUIPatternsPanel>();
    routing_panel = std::make_unique<GUIRoutingsPanel>();
    summary_panel = std::make_unique<GUISummaryPanel>();
    waveform_panel = std::make_unique<GUIWaveformPanel>();
}

GUI::~GUI() {
    stop(false);
    terminate();
}

void GUI::new_song() {
    clear_input_buffers();
    history_manager.clear();
    song.new_song();
}

void GUI::save(const std::string &filename) {
    current_path = filename;
    song.save_to_file(current_path.string());
    change_window_title(current_path.filename().string());
}

void GUI::open(const std::string &filename) {
    clear_input_buffers();
    history_manager.clear();
    song.load_from_file(filename);
    current_path = filename;

    change_window_title(current_path.filename().string());
    update();
}

void GUI::change_window_title(const std::string &title) {
    std::string window_title = APPLICATION_TITLE;
    if (!title.empty()) {
        window_title += " - " + title;
    }

    if (window != nullptr) {
        SDL_SetWindowTitle(gui.window, window_title.c_str());
    }
}

int GUI::get_jump_step() const {
    return clamp(jump_step, 0, GUI_MAX_JUMP_STEP);
}

int GUI::get_current_octave() const {
    const int min_octave = frequency_table.get_min_octave();
    const int max_octave = frequency_table.get_max_octave();
    return clamp(current_octave, min_octave, max_octave);
}

int GUI::get_page_size() const {
    return clamp(page_size, GUI_MIN_PAGE_SIZE, GUI_MAX_PAGE_SIZE);
}

int GUI::get_current_page() const {
    return patterns_panel->get_current_page();
}

int GUI::get_current_row() const {
    return patterns_panel->get_current_row();
}

std::pair<int, int> GUI::get_page_start_end(const int page) const {
    const int start = page * get_page_size();
    const int end = start + get_page_size();
    return {start, end};
}

bool GUI::initialize() {
    initialize_all();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (try_opengl_core()) {
        std::cout << "Using OpenGL Core 3.3" << std::endl;
        return true;
    }

    if (try_opengl_es()) {
        std::cout << "Using OpenGL ES 2.0" << std::endl;
        return true;
    }

#if HAVE_SDL_RENDERER
    if (try_software_renderer()) {
        std::cout << "Using Software Renderer (fallback)" << std::endl;
        return true;
    }
#endif

    std::cerr << "Failed to initialize any rendering backend" << std::endl;
    return false;
}

bool GUI::try_opengl_core() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    window = SDL_CreateWindow(
        APPLICATION_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );

    if (window == nullptr) {
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
        return false;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    return initialize_imgui_opengl("#version 330");
}

bool GUI::try_opengl_es() {
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    window = SDL_CreateWindow(
        APPLICATION_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );
    if (window == nullptr) {
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
        return false;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    return initialize_imgui_opengl("#version 100");
}

#if HAVE_SDL_RENDERER
bool GUI::try_software_renderer() {
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    window = SDL_CreateWindow(
        APPLICATION_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );
    if (!window) {
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) {
        SDL_DestroyWindow(window);
        window = nullptr;
        return false;
    }

    rendering_backend = RenderingBackend::Software;
    return initialize_imgui_software();
}
#endif

bool GUI::initialize_imgui_opengl(const char *glsl_version) {
    initialize_imgui_common();

    if (!ImGui_ImplSDL2_InitForOpenGL(window, gl_context)) {
        return false;
    }

    if (!ImGui_ImplOpenGL3_Init(glsl_version)) {
        ImGui_ImplSDL2_Shutdown();
        return false;
    }

    rendering_backend = RenderingBackend::OpenGL;
    return true;
}

bool GUI::initialize_imgui_software() {
    initialize_imgui_common();

    if (!ImGui_ImplSDL2_InitForSDLRenderer(window, renderer)) {
        return false;
    }

#if HAVE_SDL_RENDERER
    if (!ImGui_ImplSDLRenderer2_Init(renderer)) {
        ImGui_ImplSDL2_Shutdown();
        return false;
    }
#endif

    return true;
}

bool GUI::initialize_imgui_common() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();
    return true;
}

bool GUI::render() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            done = true;
        }

        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    switch (rendering_backend) {
    case RenderingBackend::OpenGL:
        return render_opengl();
    case RenderingBackend::Software:
#if HAVE_SDL_RENDERER
        return render_software();
#else
        std::cerr << "Software rendering is not supported in this build." << std::endl;
        done = true;
        return false;
#endif
    }

    return done;
}

bool GUI::render_opengl() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    frame();

    ImGui::Render();
    glViewport(0, 0, static_cast<int>(io->DisplaySize.x), static_cast<int>(io->DisplaySize.y));
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
    return done;
}

#if HAVE_SDL_RENDERER
bool GUI::render_software() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    frame();

    ImGui::Render();
    SDL_SetRenderDrawColor(renderer, 115, 140, 153, 255);
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
    return done;
}
#endif

void GUI::set_font() {
    ImFontConfig font_config;
    font_config.PixelSnapH = true;
    const float larger_font_size = 18.0f;
    font = io->Fonts->AddFontFromFileTTF("imgui/misc/fonts/ProggyClean.ttf", larger_font_size, &font_config);
}

void GUI::terminate() {
    switch (rendering_backend) {
    case RenderingBackend::OpenGL:
        if (gl_context != nullptr) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();

            SDL_GL_DeleteContext(gl_context);
            gl_context = nullptr;
        }
        break;
    case RenderingBackend::Software:
#if HAVE_SDL_RENDERER
        if (renderer) {
            ImGui_ImplSDLRenderer2_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();

            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
#endif
        break;
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

bool GUI::is_done() const {
    return done;
}

void GUI::set_audio_engine(AudioEngine *engine) {
    audio_engine = engine;
}

bool GUI::check_audio_error() const {
    if (audio_engine != nullptr) {
        return audio_engine->is_error();
    }

    return false;
}

void GUI::set_index(const GUIElement element, const int index) {
    switch (element) {
    case GUIElement::Count:
    case GUIElement::All:
    case GUIElement::Menu:
    case GUIElement::Editor:
    case GUIElement::General:
    case GUIElement::Summary:
        break;
    case GUIElement::Channels:
        channels_panel->set_index(index);
        break;
    case GUIElement::CommandsChannels:
        commands_channels_panel->set_index(index);
        break;
    case GUIElement::CommandsSequences:
        commands_sequences_panel->set_index(index);
        break;
    case GUIElement::DSPs:
        dsps_panel->set_index(index);
        break;
    case GUIElement::Envelopes:
        envelopes_panel->set_index(index);
        break;
    case GUIElement::Orders:
        orders_panel->set_index(index);
        break;
    case GUIElement::Oscillators:
        oscillators_panel->set_index(index);
        break;
    case GUIElement::Patterns:
        patterns_panel->set_index(index);
        break;
    case GUIElement::Routings:
        routing_panel->set_index(index);
        break;
    case GUIElement::Sequences:
        sequences_panel->set_index(index);
        break;
    case GUIElement::Waveform:
        waveform_panel->set_index(index);
        break;
    case GUIElement::Wavetables:
        wavetables_panel->set_index(index);
        break;
    }
}

void GUI::update(GUIElement element) {
    switch (element) {
    case GUIElement::Count:
        return;
    case GUIElement::All:
        return update_all();
    case GUIElement::Menu:
        return menu->update();
    case GUIElement::Editor:
        return editor->update();
    case GUIElement::General:
        return general_panel->update();
    case GUIElement::Channels:
        return channels_panel->update();
    case GUIElement::CommandsChannels:
        return commands_channels_panel->update();
    case GUIElement::CommandsSequences:
        return commands_sequences_panel->update();
    case GUIElement::DSPs:
        return dsps_panel->update();
    case GUIElement::Envelopes:
        return envelopes_panel->update();
    case GUIElement::Orders:
        return orders_panel->update();
    case GUIElement::Oscillators:
        return oscillators_panel->update();
    case GUIElement::Patterns:
        return patterns_panel->update();
    case GUIElement::Routings:
        return routing_panel->update();
    case GUIElement::Sequences:
        return sequences_panel->update();
    case GUIElement::Summary:
        return summary_panel->update();
    case GUIElement::Waveform:
        return waveform_panel->update();
    case GUIElement::Wavetables:
        return wavetables_panel->update();
    }
}

void GUI::initialize_all() {
    menu->initialize();
    editor->initialize();
    general_panel->initialize();
    channels_panel->initialize();
    commands_channels_panel->initialize();
    commands_sequences_panel->initialize();
    dsps_panel->initialize();
    envelopes_panel->initialize();
    orders_panel->initialize();
    oscillators_panel->initialize();
    patterns_panel->initialize();
    routing_panel->initialize();
    sequences_panel->initialize();
    summary_panel->initialize();
    waveform_panel->initialize();
    wavetables_panel->initialize();
}

void GUI::update_all() {
    menu->update();
    editor->update();
    general_panel->update();
    channels_panel->update();
    commands_channels_panel->update();
    commands_sequences_panel->update();
    dsps_panel->update();
    envelopes_panel->update();
    orders_panel->update();
    oscillators_panel->update();
    patterns_panel->update();
    routing_panel->update();
    sequences_panel->update();
    summary_panel->update();
    waveform_panel->update();
    wavetables_panel->update();
}

void GUI::frame() {
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("GeneralDock", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGuiDockNode *node = ImGui::DockBuilderGetNode(dockspace_id);
    if (node == nullptr || node->IsLeafNode()) {
        ImGui::LoadIniSettingsFromMemory(default_ini.c_str(), default_ini.size());
        std::cout << "Loading default INI settings" << std::endl;
    }

    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    shortcut_manager.process_shortcuts();
    frame_all();

    ImGui::End();
}

void GUI::frame_all() {
    menu->frame();
    editor->frame();
    general_panel->frame();
    channels_panel->frame();
    commands_channels_panel->frame();
    commands_sequences_panel->frame();
    dsps_panel->frame();
    envelopes_panel->frame();
    orders_panel->frame();
    oscillators_panel->frame();
    patterns_panel->frame();
    routing_panel->frame();
    sequences_panel->frame();
    summary_panel->frame();
    waveform_panel->frame();
    wavetables_panel->frame();
}

void GUI::from() {
    menu->from();
    editor->from();
    general_panel->from();
    channels_panel->from();
    commands_channels_panel->from();
    commands_sequences_panel->from();
    dsps_panel->from();
    envelopes_panel->from();
    orders_panel->from();
    oscillators_panel->from();
    patterns_panel->from();
    routing_panel->from();
    sequences_panel->from();
    summary_panel->from();
    waveform_panel->from();
    wavetables_panel->from();
}

void GUI::to() const {
    menu->to();
    editor->to();
    general_panel->to();
    channels_panel->to();
    commands_channels_panel->to();
    commands_sequences_panel->to();
    dsps_panel->to();
    envelopes_panel->to();
    orders_panel->to();
    oscillators_panel->to();
    patterns_panel->to();
    routing_panel->to();
    sequences_panel->to();
    summary_panel->to();
    waveform_panel->to();
    wavetables_panel->to();
}

void GUI::set_visibility_all(const bool visible) {
    menu->visible = visible;
    editor->visible = visible;
    general_panel->visible = visible;
    channels_panel->visible = visible;
    commands_channels_panel->visible = visible;
    commands_sequences_panel->visible = visible;
    dsps_panel->visible = visible;
    envelopes_panel->visible = visible;
    orders_panel->visible = visible;
    oscillators_panel->visible = visible;
    patterns_panel->visible = visible;
    routing_panel->visible = visible;
    sequences_panel->visible = visible;
    summary_panel->visible = visible;
    waveform_panel->visible = visible;
    wavetables_panel->visible = visible;
}

std::pair<ValidationResult, int> GUI::pre_play() const {
    link_manager.capture_parameters();
    const auto [result, index] = song.validate();
    if (audio_engine != nullptr && result == ValidationResult::OK) {
        audio_engine->set_output_channels(song.get_output_channels());
    }
    return {result, index};
}

std::pair<ValidationResult, int> GUI::play() const {
    const auto [result, index] = pre_play();
    if (result == ValidationResult::OK && audio_engine != nullptr) {
        if (audio_engine->is_playing()) {
            audio_engine->pause();
        } else {
            audio_engine->play();
        }
    }

    return {result, index};
}

std::pair<ValidationResult, int> GUI::play_from(const uint16_t row, const bool restart) const {
    const auto [result, index] = pre_play();
    if (result == ValidationResult::OK && audio_engine != nullptr) {
        if (restart) {
            audio_engine->stop();
            audio_engine->play(row);
        } else {
            if (audio_engine->is_playing()) {
                audio_engine->pause();
            } else {
                audio_engine->play(row);
            }
        }
    }

    return {result, index};
}

void GUI::stop(const bool restore_parameters) const {
    if (audio_engine != nullptr) {
        audio_engine->stop();
        audio_engine->clear_history();
    }

    if (restore_parameters) {
        link_manager.restore_parameters();
    }
}

bool GUI::is_playing() const {
    if (audio_engine != nullptr) {
        return audio_engine->is_playing();
    }

    return false;
}

bool GUI::is_paused() const {
    if (audio_engine != nullptr) {
        return audio_engine->is_paused();
    }

    return false;
}

bool GUI::is_fullscreen() const {
    return fullscreen;
}

const AudioHistory &GUI::get_audio_history() const {
    if (audio_engine != nullptr) {
        return audio_engine->get_history();
    }

    return empty_history;
}

void GUI::lock_audio_history() const {
    if (audio_engine != nullptr) {
        audio_engine->lock_history();
    }
}

void GUI::unlock_audio_history() const {
    if (audio_engine != nullptr) {
        audio_engine->unlock_history();
    }
}

void GUI::deselect_all_rows() {
    patterns_panel->deselect_all_rows();
}

void GUI::set_visibility(const GUIElement element, const bool visible) {
    switch (element) {
    case GUIElement::Count:
    case GUIElement::All:
        break;
    case GUIElement::Menu:
        menu->visible = visible;
        break;
    case GUIElement::Editor:
        editor->visible = visible;
        break;
    case GUIElement::General:
        general_panel->visible = visible;
        break;
    case GUIElement::Channels:
        channels_panel->visible = visible;
        break;
    case GUIElement::CommandsChannels:
        commands_channels_panel->visible = visible;
        break;
    case GUIElement::CommandsSequences:
        commands_sequences_panel->visible = visible;
        break;
    case GUIElement::DSPs:
        dsps_panel->visible = visible;
        break;
    case GUIElement::Envelopes:
        envelopes_panel->visible = visible;
        break;
    case GUIElement::Orders:
        orders_panel->visible = visible;
        break;
    case GUIElement::Oscillators:
        oscillators_panel->visible = visible;
        break;
    case GUIElement::Patterns:
        patterns_panel->visible = visible;
        break;
    case GUIElement::Routings:
        routing_panel->visible = visible;
        break;
    case GUIElement::Sequences:
        sequences_panel->visible = visible;
        break;
    case GUIElement::Summary:
        summary_panel->visible = visible;
        break;
    case GUIElement::Waveform:
        waveform_panel->visible = visible;
        break;
    case GUIElement::Wavetables:
        wavetables_panel->visible = visible;
        break;
    }
}

bool GUI::get_visibility(const GUIElement element) const {
    switch (element) {
    case GUIElement::Count:
    case GUIElement::All:
        return false;
    case GUIElement::Menu:
        return menu->visible;
    case GUIElement::Editor:
        return editor->visible;
    case GUIElement::General:
        return general_panel->visible;
    case GUIElement::Channels:
        return channels_panel->visible;
    case GUIElement::CommandsChannels:
        return commands_channels_panel->visible;
    case GUIElement::CommandsSequences:
        return commands_sequences_panel->visible;
    case GUIElement::DSPs:
        return dsps_panel->visible;
    case GUIElement::Envelopes:
        return envelopes_panel->visible;
    case GUIElement::Orders:
        return orders_panel->visible;
    case GUIElement::Oscillators:
        return oscillators_panel->visible;
    case GUIElement::Patterns:
        return patterns_panel->visible;
    case GUIElement::Routings:
        return routing_panel->visible;
    case GUIElement::Sequences:
        return sequences_panel->visible;
    case GUIElement::Summary:
        return summary_panel->visible;
    case GUIElement::Waveform:
        return waveform_panel->visible;
    case GUIElement::Wavetables:
        return wavetables_panel->visible;
    }

    return false;
}

void GUI::clear_input_buffers() {
    commands_sequences_panel->clear_input_buffers();
}

int GUI::get_current_channel_index() const {
    return channels_panel->channel_index;
}

int GUI::get_current_dsp_index() const {
    return dsps_panel->dsp_index;
}

int GUI::get_current_commands_channel_index() const {
    return commands_channels_panel->channel_index;
}

int GUI::get_current_oscillator_index() const {
    return oscillators_panel->oscillator_index;
}

int GUI::get_current_envelope_index() const {
    return envelopes_panel->envelope_index;
}

int GUI::get_current_sequence_index() const {
    return sequences_panel->sequence_index;
}

int GUI::get_current_order_index() const {
    return orders_panel->order_index;
}

int GUI::get_current_wavetable_index() const {
    return wavetables_panel->wavetable_index;
}

int GUI::get_current_commands_sequence_index() const {
    return commands_sequences_panel->sequence_index;
}

void GUI::set_current_octave(const int octave) {
    current_octave = octave;
}

void GUI::set_jump_step(const int step) {
    jump_step = step;
}

void GUI::set_page_size(const int size) {
    page_size = size;
}

void GUI::set_current_channel_index(const int index) {
    channels_panel->set_index(index);
}

void GUI::set_current_dsp_index(const int index) {
    dsps_panel->set_index(index);
}

void GUI::set_current_commands_channel_index(const int index) {
    commands_channels_panel->set_index(index);
}

void GUI::set_current_oscillator_index(const int index) {
    oscillators_panel->set_index(index);
}

void GUI::set_current_envelope_index(const int index) {
    envelopes_panel->set_index(index);
}

void GUI::set_current_sequence_index(const int index) {
    sequences_panel->set_index(index);
}

void GUI::set_current_order_index(const int index) {
    orders_panel->set_index(index);
}

void GUI::set_current_wavetable_index(const int index) {
    wavetables_panel->set_index(index);
}

void GUI::set_current_commands_sequence_index(const int index) {
    commands_sequences_panel->set_index(index);
}

void GUI::clear_routing_nodes() {
    routing_panel->clear_nodes();
}

std::vector<std::pair<NodeIdentifier, ImVec2>> GUI::get_routing_nodes_positions() const {
    return routing_panel->get_nodes_positions();
}

void GUI::set_routing_nodes_positions(const std::vector<std::pair<NodeIdentifier, ImVec2>> &nodes_positions) {
    routing_panel->set_nodes_positions(nodes_positions);
}

bool GUI::is_pattern_view_active() const {
    return patterns_panel->is_active() ||
           commands_sequences_panel->is_active() ||
           sequences_panel->is_active();
}

bool GUI::is_commands_pattern_view_active() const {
    return commands_sequences_panel->is_active() ||
           (patterns_panel->is_active() && patterns_panel->is_commands_view_active());
}

void GUI::toggle_fullscreen() {
    if (window == nullptr) {
        return;
    }

    fullscreen = !fullscreen;

    if (fullscreen) {
        if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0) {
            std::cerr << "Failed to enter fullscreen: " << SDL_GetError() << std::endl;
            fullscreen = false;
        }
    } else {
        if (SDL_SetWindowFullscreen(window, 0) != 0) {
            std::cerr << "Failed to exit fullscreen: " << SDL_GetError() << std::endl;
            fullscreen = true;
        }
    }
}
