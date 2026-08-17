#pragma once
#include "Components.hpp"
#include <entt/entt.hpp>

namespace ProgressBarSystems {
    void update(entt::registry& registry);
    void render(entt::registry& registry, sf::RenderWindow& window);
}