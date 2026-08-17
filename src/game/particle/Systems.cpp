#include "Systems.hpp"

namespace ParticleSystems {
    void update(entt::registry& registry, float dt) {
        auto view = registry.view<Particle>();

        for (auto [entity, partilce] : view.each()) {
            partilce.time_elapsed += dt;
            if (partilce.time_elapsed >= partilce.lifetime) {
                registry.destroy(entity);
            }
        }
    }
}