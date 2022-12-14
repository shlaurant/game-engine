#pragma once

#include "assets/registry.h"
#include "ecs/systems/sprite_renderer_system.h"
#include "ecs/systems/text_renderer_system.h"
#include "ecs/systems/frame_animation_system.h"
#include "ecs/systems/tilemap_renderer_system.h"
#include "ecs/systems/rigidbody_system.h"
#include "ecs/systems/collision_system.h"
#include "ecs/systems/script_system.h"

#include "serializer.h"
#include "assets/serializer.h"

namespace fuse::ecs {
    class scene {
    public:
        FUSE_INLINE explicit scene(SDL_Renderer *rd) : _renderer(rd) {
            register_system<ecs::sprite_renderer_system>();
            register_system<ecs::text_renderer_system>();
            register_system<ecs::frame_animation_system>();
            register_system<ecs::tilemap_renderer_system>();
            register_system<ecs::rigidbody_system>();
            register_system<ecs::collision_system>();
            register_system<ecs::script_system>();
        }

        FUSE_INLINE ~scene() {
            for (auto &s: _systems) { FUSE_DELETE(s); }
            _registry.clear();
            _systems.clear();
        }

        FUSE_INLINE ecs::entity add_entity(const std::string &name) {
            auto entity = ecs::entity(&_registry);
            entity.add_component<info_component>().name = name;
            entity.add_component<transform_component>();
            return entity;
        }

        FUSE_INLINE void update(float dt) {
            for (auto &sys: _systems) { sys->update(dt); }
        }

        FUSE_INLINE void start() {
//            auto dead = _assets.load_texture("assets/dead.png", "dead",
//                                             _renderer);
//            auto fly = _assets.load_texture("assets/fly.png", "fly", _renderer);
//            auto pipe = _assets.load_texture("assets/pipe.png", "pipe",
//                                             _renderer);
//            auto ground = _assets.load_texture("assets/ground.png", "ground",
//                                               _renderer);
//            auto background = _assets.load_texture("assets/bg.png", "bg",
//                                                   _renderer);
//            auto font = _assets.load_font("assets/font.ttf", "font", 30);
//
//            auto bg = add_entity("background");
//            bg.add_component<sprite_component>().sprite = background->id;
//
//            auto gd = add_entity("ground");
//            auto &gds = gd.add_component<ecs::script_component>();
//            gds.bind<scrolling_ground>();
//            gds.name = "scrolling_ground";
//            auto &gd_tr = gd.get_component<ecs::transform_component>();
//            gd_tr.translate = vec2f(0.0f, 620);
//            gd.add_component<rigidbody_component>().body.velocity.x = -100.0f;
//            auto &gd_sp = gd.add_component<sprite_component>().sprite = ground->id;
//            auto &gd_cl = gd.add_component<collider_component>();
//            gd_cl.collider = {0, 0, (float) ground->instance.width,
//                              (float) ground->instance.height};
//
//
//            auto player = add_entity("player");
//            auto &ps = player.add_component<script_component>();
//            ps.bind<player_controller>();
//            ps.name = "player_controller";
//            auto &tr = player.get_component<transform_component>();
//            tr.translate = vec2f(126, 360);
//            tr.scale = vec2f(0.5f);
//            auto &rb = player.add_component<rigidbody_component>();
//            rb.body.gravity_scale = 25;
//            player.add_component<sprite_component>().sprite = fly->id;
//            auto &cl = player.add_component<collider_component>();
//            cl.collider = {0, 0, 58, 58};
//
//            auto spawner = add_entity("pipe_spawner");
//            auto &sc = spawner.add_component<script_component>();
//            sc.bind<pipe_spawner>();
//            sc.name = "pipe_spawner";
//
//            auto score = add_entity("score");
//            auto &s_tr = score.get_component<transform_component>();
//            s_tr.translate = vec2f(120, 20);
//            auto &tx = score.add_component<text_component>();
//            tx.text = "Score: 0";
//            tx.font = font->id;
//
//            _assets.load_audio("assets/song.mp3", "music");
//            _assets.load_audio("assets/boom.wav", "boom");

            for (auto &sys: _systems) { sys->start(); }
        }

        template<typename T>
        FUSE_INLINE void register_system() {
            FUSE_STATIC_ASSERT(std::is_base_of<ecs::system, T>::value);
            auto new_system = new T();
            new_system->prepare(&_registry, _renderer, &_assets);
            _systems.push_back(new_system);
        }

        FUSE_INLINE void serialize(const std::string &path) {
            YAML::Emitter em;
            em << YAML::BeginMap;
            asset_serializer(&_assets).serialize(em);
            ecs::serializer(&_registry).serialize(em);
            em << YAML::EndMap;

            std::ofstream filepath(path);
            filepath << em.c_str();
        }

        FUSE_INLINE bool deserialize(const std::string &path) {
            YAML::Node root;
            try {
                root = YAML::LoadFile(path);
            } catch (YAML::ParserException &e) {
                FUSE_INFO("failed to deserialize scene!")
                return false;
            }

            if (auto assets = root["assets"]) {
                asset_serializer(&_assets).deserialize(assets, _renderer);
            }

            if (auto entities = root["entities"]) {
                ecs::serializer(&_registry).deserialize(entities);
            }

            for (auto &sys: _systems) { sys->start(); }

            return true;
        }

    private:
        std::vector<ecs::system *> _systems;
        SDL_Renderer *_renderer = nullptr;
        ecs::registry _registry;
        asset_registry _assets;
    };
}