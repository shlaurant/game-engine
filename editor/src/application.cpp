#include <cstdlib>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <SDL2/SDL.h>
#include "event/core.h"
#include "gui/gui.h"

int main(int arg, char **argv) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        exit(EXIT_FAILURE);
    }

    auto wflags = (SDL_WindowFlags) SDL_WINDOW_RESIZABLE |
                  SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_MAXIMIZED;
    auto window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, 960, 640, wflags);

    auto r_flags = (SDL_RendererFlags) (SDL_RENDERER_ACCELERATED |
                                        SDL_RENDERER_PRESENTVSYNC);
    auto renderer = SDL_CreateRenderer(window, -1, r_flags);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    auto disp = std::make_shared<editor::dispatcher>();
//    auto data = std::make_shared<editor::scene_data>();

    // Main loop
    bool done = false;
    while (!done) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        editor::gui::gui::instance()->show();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // Rendering
        ImGui::Render();
        SDL_SetRenderDrawColor(renderer, (Uint8) (clear_color.x * 255),
                               (Uint8) (clear_color.y * 255),
                               (Uint8) (clear_color.z * 255),
                               (Uint8) (clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);

        disp->dispatch();
    }

    // Cleanup
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}