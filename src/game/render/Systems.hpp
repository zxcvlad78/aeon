#pragma once
#include "./Components.hpp"

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace RenderSystems  {
    void update(entt::registry& registry, sf::RenderWindow& window);
    void draw_hitboxes(entt::registry& registry, sf::RenderWindow& window);

    extern bool enabled;
    extern bool draw_hitboxes_enabled;
}