#pragma once
#include "Components.hpp"

void player_input_system(entt::registry& registry);
void movement_system(entt::registry& registry, float dt);
void camera_system(entt::registry& registry, sf::RenderWindow& window, float dt);

void sprite_animation_control_system(entt::registry& registry);
void sprite_animation_system(entt::registry& registry, float dt);

void projectile_system(entt::registry& registry, float dt);
void render_system(entt::registry& registry, sf::RenderWindow& window);