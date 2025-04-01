#include <algorithm>

#include "gui.hpp"

GUI::GUI()
    : window(nullptr), gl_context(nullptr) {
}

GUI::~GUI() {
    terminate();
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

    menu.draw();
    editor.draw();
    channels_panel.draw();
    envelopes_panel.draw();
    orders_panel.draw();
    oscillators_panel.draw();
    sequences_panel.draw();
    wavetables_panel.draw();
    general_panel.draw();

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

void GUI::set_play_callback(std::function<void()> callback) {
    general_panel.play_callback = callback;
}

void GUI::set_playing_status(bool status) {
    general_panel.is_playing = status;
}

void GUI::update() {
    channels_panel.update();
    envelopes_panel.update();
    orders_panel.update();
    oscillators_panel.update();
    sequences_panel.update();
    wavetables_panel.update();
}
