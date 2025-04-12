#include <algorithm>

#include "../general.hpp"
#include "gui.hpp"

GUI::GUI()
    : window(nullptr), gl_context(nullptr) {
}

GUI::~GUI() {
    terminate();
}

void GUI::change_window_title(const std::string &title) {
    std::string window_title = APPLICATION_TITLE;
    if (!title.empty()) {
        window_title += " - " + title;
    }

    if (window) {
        SDL_SetWindowTitle(gui.window, window_title.c_str());
    }
}

int GUI::get_jump_step() const {
    return std::clamp(jump_step, 0, GUI_MAX_JUMP_STEP);
}

int GUI::get_current_octave() const {
    const int min_octave = frequency_table.get_min_octave();
    const int max_octave = frequency_table.get_max_octave();
    return std::clamp(current_octave, min_octave, max_octave);
}

int GUI::get_page_size() const {
    return std::clamp(page_size, GUI_MIN_PAGE_SIZE, GUI_MAX_PAGE_SIZE);
}

bool GUI::initialize() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    window = SDL_CreateWindow(APPLICATION_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GUI_WINDOW_WIDTH, GUI_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        printf("Failed to create SDL window: %s\n", SDL_GetError());
        return false;
    }

    gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        printf("Failed to create OpenGL ES context: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = &ImGui::GetIO();
    (void) io;

    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // set_font();

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 100");

    return true;
}

bool GUI::render() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            done = true;
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    frame();

    ImGui::Render();
    glViewport(0, 0, (int) io->DisplaySize.x, (int) io->DisplaySize.y);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
    return done;
}

void GUI::set_font() {
    ImFontConfig font_config;
    font_config.PixelSnapH = true;
    const float larger_font_size = 18.0f;
    font = io->Fonts->AddFontFromFileTTF("imgui/misc/fonts/ProggyClean.ttf", larger_font_size, &font_config);
}

void GUI::terminate() {
    if (gl_context) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(gl_context);
        gl_context = nullptr;
    }

    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

bool GUI::is_done() {
    return done;
}

void GUI::set_audio_engine(AudioEngine *engine) {
    audio_engine = engine;
}

void GUI::update(GUIElement element) {
    if (element == GUIElement::All) {
        return update_all();
    }

    switch (element) {
    case GUIElement::Menu:
        return menu.update();
    case GUIElement::Editor:
        return editor.update();
    case GUIElement::General:
        return general_panel.update();
    case GUIElement::Channels:
        return channels_panel.update();
    case GUIElement::DSPs:
        return dsps_panel.update();
    case GUIElement::Envelopes:
        return envelopes_panel.update();
    case GUIElement::Orders:
        return orders_panel.update();
    case GUIElement::Oscillators:
        return oscillators_panel.update();
    case GUIElement::Sequences:
        return sequences_panel.update();
    case GUIElement::Wavetables:
        return wavetables_panel.update();
    case GUIElement::Patterns:
        return patterns_panel.update();
    }
}

void GUI::update_all() {
    menu.update();
    editor.update();
    general_panel.update();
    channels_panel.update();
    dsps_panel.update();
    envelopes_panel.update();
    orders_panel.update();
    oscillators_panel.update();
    sequences_panel.update();
    wavetables_panel.update();
    patterns_panel.update();
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
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    frame_all();

    ImGui::End();
}

void GUI::frame_all() {
    menu.frame();
    editor.frame();
    general_panel.frame();
    channels_panel.frame();
    dsps_panel.frame();
    envelopes_panel.frame();
    orders_panel.frame();
    oscillators_panel.frame();
    sequences_panel.frame();
    wavetables_panel.frame();
    patterns_panel.frame();
}

void GUI::play() const {
    if (audio_engine != nullptr) {
        if (audio_engine->is_playing()) {
            audio_engine->pause();
        } else {
            audio_engine->play();
        }
    }
}

void GUI::stop() const {
    if (audio_engine) {
        audio_engine->stop();
    }
}

bool GUI::is_playing() const {
    if (audio_engine) {
        return audio_engine->is_playing();
    }

    return false;
}

bool GUI::is_paused() const {
    if (audio_engine) {
        return audio_engine->is_paused();
    }

    return false;
}

void GUI::deselect_all_rows() {
    gui.patterns_panel.deselect_all_rows();
}