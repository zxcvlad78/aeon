#pragma once
#include <entt/entt.hpp>
#include "Components.hpp"

void input_system(entt::registry& registry);
void movement_system(entt::registry& registry, float dt);
void camera_system(entt::registry& registry, sf::RenderWindow& window, float dt);
void render_system(entt::registry& registry, sf::RenderWindow& window);
void sprite_animation_system(entt::registry& registry, float dt);
void sprite_animation_control_system(entt::registry& registry);