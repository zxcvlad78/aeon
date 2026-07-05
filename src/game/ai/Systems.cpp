#include "Systems.hpp"


namespace AISystems {
    void update(entt::registry& registry, float dt) {
        ai_target(registry, dt);
    }

    void fixed_update(entt::registry& registry, float dt) {

    }

    void ai_target(entt::registry& registry, float dt) {
        auto view = registry.view<AIComponents::AITarget>();
        for (auto [entity, ai_target] : view.each()) {
            //printf("%f, %f", ai_target.time_elapsed, ai_target.update_interval);
            if (!ai_target.should_pick()) {
                ai_target.time_elapsed += dt;
            }
        }
    }


};