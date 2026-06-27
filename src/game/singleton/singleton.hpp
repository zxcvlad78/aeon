#pragma once
#include <entt/entt.hpp>
#include "../../../include/Components.hpp"

namespace Singleton {
    entt::entity spawn_projectile(
    entt::registry& registry,
    Projectile      projectile,
    Transform       transform,
    Velocity        velocity,
    std::string     atlas_path = "res/textures/t_projectile/atlas.png",
    std::string     spritesheet_path = "res/t_projectile_spritesheet.json");

    entt::entity spawn_enemy(
        entt::registry& registry,
        std::string sprite_atlas_path,
        std::string spritesheet_path,
        std::string projectile_atlas_path,
        std::string projectile_spritesheet_path,
        std::string projectile_hitsound
    );
}