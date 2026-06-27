#pragma once
#include "../faction/Components.hpp"
#include "Components.hpp"

void mob_system(entt::registry& registry, float dt);

void mob_movement_system(entt::registry& registry, float dt);
void mob_attack_ranged_system(entt::registry& registry, float dt);
void mob_spawner_system(entt::registry& registry, float dt);

namespace MobFunc {
    std::vector<entt::entity> get_enemies(entt::registry& registry, entt::entity from);

    std::vector<entt::entity> get_allies(entt::registry& registry, entt::entity from);

    entt::entity get_nearest_enemy(entt::registry& registry, entt::entity from);

    entt::entity get_nearest_ally(entt::registry& registry, entt::entity from);
}