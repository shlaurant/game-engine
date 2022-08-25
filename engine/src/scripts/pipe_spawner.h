#pragma once

#include "math/random.h"
#include "script_instance.h"

namespace fuse {
    class pipe_spawner : public script_instance {
    public:
        FUSE_INLINE void on_start() override {
            play_audio("music", 1, -1);
        }

        FUSE_INLINE void on_update(float dt) override {

            if (game_over) { return; }

            auto player = find_entity("player");
            auto &p_rb = player.get_component<ecs::rigidbody_component>();

            if (player.is_alive() && p_rb.disabled) {
                auto ground = find_entity("ground");
                auto &gd = ground.get_component<ecs::rigidbody_component>();
                gd.disabled = true;

                auto scr = find_entity("score");
                auto &tx = scr.get_component<ecs::text_component>();
                tx.text = "Game Over";

                for (auto &pipe: pipes) {
                    auto &rb = pipe.get_component<ecs::rigidbody_component>();
                    rb.disabled = true;
                    auto &co = pipe.get_component<ecs::collider_component>();
                    co.disabled = true;
                }

                game_over = true;
                return;
            }

            update_pipe();

            if (time > 2.5f) {
                time = 0.0f;
                float space = math::random(60, 150);
                spawn_pipe(380, -space);
                spawn_pipe(380, 720 / 2 + space);
            }

            time += dt;
        }

        FUSE_INLINE void spawn_pipe(float x, float y) {
            auto pipe = add_entity();
            auto &tr = pipe.get_component<ecs::transform_component>();
            tr.translate = vec2f(x, y);

            auto &rb = pipe.add_component<ecs::rigidbody_component>();
            rb.body.gravity_scale = 0;
            rb.body.velocity.x = -100;

            auto pipe_sprite = get_asset<texture_asset>("pipe");
            auto &sp = pipe.add_component<ecs::sprite_component>();
            sp.flip = (y < 0) ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;
            sp.sprite = pipe_sprite->id;

            auto &cl = pipe.add_component<ecs::collider_component>();
            cl.collider = {
                    tr.translate.x,
                    tr.translate.y,
                    (float) pipe_sprite->instance.width,
                    (float) pipe_sprite->instance.height
            };
            pipes.push_back(pipe);
        }

        FUSE_INLINE void update_pipe() {
            auto it = pipes.begin();

            while (it != pipes.end()) {
                auto &pipe = *it;

                if (!pipe.is_alive()) {
                    it = pipes.erase(it);
                    continue;
                }

                auto &tr = pipe.get_component<ecs::transform_component>();
                if (tr.translate.x < -50) {
                    pipe.destroy();
                    auto &tx = find_entity(
                            "score").get_component<ecs::text_component>();
                    tx.text = "Score: " + std::to_string(++score);
                }

                it++;
            }
        }

    private:
        std::vector<ecs::entity> pipes;
        bool game_over = false;
        float time = 0;
        int score = 0;
    };
}