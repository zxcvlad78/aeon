#pragma once
#include "./Components.hpp"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace RenderSystems  {
    void update(entt::registry& registry, sf::RenderWindow& window);
    void render_hitboxes(entt::registry& registry, sf::RenderWindow& window);

    extern bool enabled;
    extern bool render_hitboxes_enabled;
}