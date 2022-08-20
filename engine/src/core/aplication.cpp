#include "pch.h"
#include "application.h"
#include "inputs.h"
#include "events/system.h"

namespace fuse {
    static bool is_running = true;

    FUSE_INLINE bool on_quit(const quite_event &) {
        return is_running = false;
    }

    FUSE_API void run_application(const app_config &config) {
        if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
            FUSE_ERROR("%s", SDL_GetError())
            exit(EXIT_FAILURE);
        }

        if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) < 0) {
            FUSE_ERROR("%s", IMG_GetError())
        }

        if (TTF_Init() < 0) {
            FUSE_ERROR("%s", TTF_GetError())
        }

        if (Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3) < 0 ||
            Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512)) {
            FUSE_ERROR("%s", TTF_GetError())
        }

        auto w_flags = (SDL_WindowFlags) (SDL_WINDOW_RESIZABLE |
                                          SDL_WINDOW_ALLOW_HIGHDPI);
        auto window = SDL_CreateWindow(config.title.c_str(),
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, config.width,
                                       config.height, w_flags);

        auto r_flags = (SDL_RendererFlags) (SDL_RENDERER_ACCELERATED |
                                            SDL_RENDERER_PRESENTVSYNC);
        auto renderer = SDL_CreateRenderer(window, -1, r_flags);

        if (!window || !renderer) {
            FUSE_ERROR("%s", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        inputs::initialize(window);
        inputs::get_dispatcher()->add_callback<quite_event>(on_quit);

        while (is_running) {
            inputs::dispatch_events();
            if (inputs::is_button(SDL_BUTTON_LEFT)) {
                auto mouse = inputs::mouse_offset();
                FUSE_INFO("(%f, %f)", mouse.x, mouse.y);
            }
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
    }
}