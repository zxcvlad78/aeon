#pragma once
#include "Components.hpp"

namespace AISystems {
    void update(entt::registry& registry, float dt);
    void fixed_update(entt::registry& registry, float dt);

    void ai_target(entt::registry& registry, float dt);
};