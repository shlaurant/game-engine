#pragma once
#include "entity.h"
#include "assets/registry.h"
#include "ecs/components/graphics.h"
#include "ecs/components/common.h"
#include "ecs/components/physics.h"
#include "ecs/components/behaviour.h"

namespace fuse::ecs {
    class system {
    public:
        FUSE_INLINE virtual ~system() = default;

        FUSE_INLINE void prepare(registry *rg, SDL_Renderer *rd, asset_registry *as) {
            _registry = rg;
            _renderer = rd;
            _assets = as;
        }

        /// Get vector of entities having certain component T
        /// \tparam T Inherits component
        /// \return Vector of entities having component of type T
        template<typename T>
        FUSE_INLINE std::vector<entity> view(){
            std::vector<entity> entities;
            for(auto &e:_registry->view<T>()){
                entities.push_back(entity(e, _registry));
            }
            return entities;
        }

        FUSE_INLINE virtual void update(float) {}
        FUSE_INLINE virtual void start() {}

    protected:
        SDL_Renderer *_renderer = nullptr;
        registry *_registry = nullptr;
        asset_registry *_assets = nullptr;
    };
}