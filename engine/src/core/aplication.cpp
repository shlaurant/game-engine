#include <ecs/scene.h>
#include <SDL2/SDL_syswm.h>
#include "pch.h"
#include "application.h"
#include "inputs.h"
#include "events/system.h"

namespace fuse {
    static app_config config;
    static bool is_running = true;
    static ecs::scene *scene = nullptr;
    static float deltatime, last_tick;

    FUSE_INLINE bool on_quit(const quite_event &) {
        return is_running = false;
    }

    FUSE_INLINE bool on_key(const keydown_event &e){
        if(e.key == SDL_SCANCODE_R){
            std::cout << "DFDF";
            scene->deserialize(config.scene);
        }
        return false;
    }

    FUSE_INLINE void compute_deltatime() {
        deltatime = get_ticks() - last_tick;
        if (deltatime > MAX_DELTATIME) {
            deltatime = MAX_DELTATIME;
        }
        last_tick = get_ticks();
    }

    FUSE_API void run_application(const app_config &cfg) {
        config = cfg;

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
        auto window = SDL_CreateWindow(cfg.title.c_str(),
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED, cfg.width,
                                       cfg.height, w_flags);

        auto r_flags = (SDL_RendererFlags) (SDL_RENDERER_ACCELERATED |
                                            SDL_RENDERER_PRESENTVSYNC);
        auto renderer = SDL_CreateRenderer(window, -1, r_flags);

        if (!window || !renderer) {
            FUSE_ERROR("%s", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        SDL_SysWMinfo wm_info;
        SDL_VERSION(&wm_info.version)
        SDL_GetWindowWMInfo(window, &wm_info);
        auto hwnd = wm_info.info.win.window;

        inputs::initialize(window);
        auto disp = inputs::get_dispatcher();
        disp->add_callback<quite_event>(on_quit);
        disp->add_callback<keydown_event>(on_key);

        scene = new ecs::scene(renderer);
        scene->deserialize(config.scene);

//        scene->start();
//        scene->serialize("assets/scene.yaml");


        last_tick = get_ticks();

        while (is_running) {
            compute_deltatime();
            inputs::dispatch_events();
            SDL_RenderClear(renderer);
            scene->update(deltatime);
            SDL_RenderPresent(renderer);
        }

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        FUSE_DELETE(scene);
        IMG_Quit();
        Mix_Quit();
        TTF_Quit();
        SDL_Quit();
    }
}