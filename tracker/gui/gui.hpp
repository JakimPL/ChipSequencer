#ifndef GUI_GUI_HPP
#define GUI_GUI_HPP

#include <atomic>
#include <functional>
#include "init.hpp"
#include "envelopes.hpp"

class GUI {
  public:
    GUI()
        : window(nullptr), gl_context(nullptr), play_callback(nullptr), is_playing(false) {}

    ~GUI() {
        terminate();
    }

    bool initialize() {
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

        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init("#version 100");

        return true;
    }

    bool render() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                done = true;
            ImGui_ImplSDL2_ProcessEvent(&event);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Control Panel");
        envelope_panel.draw();
        if (draw_play_button() && !is_playing && play_callback) {
            is_playing = true;
            play_callback();
        }
        if (is_playing) {
            ImGui::Text("Playing audio...");
        }
        ImGui::End();

        ImGui::Render();
        glViewport(0, 0, (int) io->DisplaySize.x, (int) io->DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
        return done;
    }

    void terminate() {
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

    bool is_done() {
        return done;
    }

    void set_play_callback(std::function<void()> callback) {
        play_callback = callback;
    }

    void set_playing_status(bool status) {
        is_playing = status;
    }

  private:
    SDL_Window *window;
    SDL_GLContext gl_context;
    ImGuiIO *io;
    bool done = false;
    std::function<void()> play_callback;
    std::atomic<bool> is_playing;
    EnvelopeGUI envelope_panel;

    bool draw_play_button() {
        ImVec2 p = ImGui::GetCursorScreenPos();
        float sz = 50.0f;
        ImDrawList *draw_list = ImGui::GetWindowDrawList();
        ImVec2 center = ImVec2(p.x + sz * 0.5f, p.y + sz * 0.5f);

        ImVec2 points[3] = {
            ImVec2(center.x - sz * 0.3f, center.y - sz * 0.5f),
            ImVec2(center.x + sz * 0.5f, center.y),
            ImVec2(center.x - sz * 0.3f, center.y + sz * 0.5f)};

        draw_list->AddTriangleFilled(points[0], points[1], points[2], IM_COL32(0, 255, 0, 255)); // Green triangle

        ImGui::SetCursorScreenPos(p);
        ImGui::InvisibleButton("PlayButton", ImVec2(sz, sz));
        return ImGui::IsItemClicked();
    }
};

#endif // GUI_GUI_HPP
