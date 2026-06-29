#pragma once
#include <entt/entt.hpp>
#include "../../../include/Components.hpp"


namespace Singleton {
    namespace Variables {
        extern sf::Font main_font;
        const std::string GAME_VERSION = "v0.0.1";
        const sf::Vector2u WINDOW_SIZE = sf::Vector2u(1280, 720);
    }

    entt::entity spawn_projectile(
        entt::registry& registry,
        Projectile      projectile,
        Transform       transform,
        Velocity        velocity,
        std::string     atlas_path = "res/textures/t_projectile/atlas.png",
        std::string     spritesheet_path = "res/t_projectile_spritesheet.json"
    );

    entt::entity spawn_enemy(
            entt::registry& registry,
            std::string sprite_atlas_path,
            std::string spritesheet_path,
            std::string projectile_atlas_path,
            std::string projectile_spritesheet_path,
            std::string projectile_hitsound
    );
}