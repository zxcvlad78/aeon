#pragma once
#include "./Components.hpp"

namespace SpriteSystems {
    void update(entt::registry& registry, sf::RenderWindow& window, float dt);
    void render(entt::registry& registry, sf::RenderWindow& window);
    void update_animation(entt::registry& registry, float dt);
}