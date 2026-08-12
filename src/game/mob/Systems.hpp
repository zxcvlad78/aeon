#pragma once

#include "Components.hpp"
#include "../faction/Components.hpp"

namespace MobSystems {
    void update(entt::registry& registry, float dt);
    
    void movement(entt::registry& registry, float dt);
    void attack_ranged(entt::registry& registry, float dt);

    extern bool enabled;
}


namespace MobFunc {
    std::vector<entt::entity> get_enemies(entt::registry& registry, entt::entity from);

    std::vector<entt::entity> get_allies(entt::registry& registry, entt::entity from);

    entt::entity get_nearest_enemy(entt::registry& registry, entt::entity from);

    entt::entity get_nearest_ally(entt::registry& registry, entt::entity from);
}