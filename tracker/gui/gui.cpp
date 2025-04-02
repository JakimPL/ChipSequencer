#include <algorithm>

#include "gui.hpp"

GUI::GUI()
    : window(nullptr), gl_context(nullptr) {
}

GUI::~GUI() {
    terminate();
}

int GUI::get_jump_step() const {
    return std::clamp(0, jump_step, GUI_MAX_JUMP_STEP);
}

int GUI::get_current_octave() const {
    return std::clamp(0, current_octave, TUNING_MAX_OCTAVE);
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

    window = SDL_CreateWindow("ChipSequencer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
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

    menu.frame();
    editor.frame();
    channels_panel.frame();
    envelopes_panel.frame();
    orders_panel.frame();
    oscillators_panel.frame();
    sequences_panel.frame();
    wavetables_panel.frame();
    general_panel.frame();

    ImGui::Render();
    glViewport(0, 0, (int) io->DisplaySize.x, (int) io->DisplaySize.y);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
    return done;
}

void GUI::set_font() {
    ImFontConfig fontConfig;
    fontConfig.PixelSnapH = true;
    float fontSize = 13.0f;
    float largerFontSize = 18.0f;
    font = io->Fonts->AddFontFromFileTTF("imgui/misc/fonts/ProggyClean.ttf", largerFontSize, &fontConfig);
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
    }
}

void GUI::update_all() {
    menu.update();
    editor.update();
    general_panel.update();
    channels_panel.update();
    envelopes_panel.update();
    orders_panel.update();
    oscillators_panel.update();
    sequences_panel.update();
    wavetables_panel.update();
}

void GUI::play() const {
    if (audio_engine != nullptr) {
        if (audio_engine->is_playing()) {
            audio_engine->pause();
            return;
        }
        audio_engine->play();
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
